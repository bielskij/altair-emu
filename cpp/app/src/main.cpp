/*
 * MIT License
 *
 * Copyright (c) 2020 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <signal.h>
#include <unistd.h>

#include "common/Ini.hpp"

#include "altair/Config.hpp"
#include "altair/MainBoard.hpp"
#include "altair/Card/Cpu.hpp"
#include "altair/Card/Mcs16.hpp"
#include "altair/Card/Sio.hpp"
#include "altair/Card/VI.hpp"
#include "altair/Card/Pmc.hpp"

#include "altair/Card/devel/Writer.hpp"
#include "altair/Card/devel/Reader.hpp"

#include "altair/utils/ImageLoaderFactory.hpp"


#define DEBUG_LEVEL 5
#include "common/debug.h"


static altair::card::Cpu::ClkSource *clk = nullptr;


static void _signalHandler(int signo) {
	clk->stop();
}


static altair::card::Mcs16::Sw1 stringToSw1(const std::string &str) {
	if (str == "0") return altair::card::Mcs16::Sw1::COOO;
	if (str == "1") return altair::card::Mcs16::Sw1::OCOO;
	if (str == "2") return altair::card::Mcs16::Sw1::OOCO;
	if (str == "3") return altair::card::Mcs16::Sw1::OOOC;

	throw std::invalid_argument("Invalid bank number!");
}


static altair::card::Sio::BaudRate stringToSioBaud(const std::string &str) {
	if (str == "110")   return altair::card::Sio::B110;
	if (str == "150")   return altair::card::Sio::B150;
	if (str == "300")   return altair::card::Sio::B300;
	if (str == "600")   return altair::card::Sio::B600;
	if (str == "1200")  return altair::card::Sio::B1200;
	if (str == "2400")  return altair::card::Sio::B2400;
	if (str == "4800")  return altair::card::Sio::B4800;
	if (str == "9600")  return altair::card::Sio::B9600;
	if (str == "19200") return altair::card::Sio::B19200;

	throw std::invalid_argument("Invalid baudrate value!");
}


static altair::card::Sio::IntLevel stringToSioIntLevel(const std::string &str) {
	if (str == "sl")  return altair::card::Sio::IntLevel::SL;
	if (str == "vi0") return altair::card::Sio::IntLevel::VI0;
	if (str == "vi1") return altair::card::Sio::IntLevel::VI1;
	if (str == "vi2") return altair::card::Sio::IntLevel::VI2;
	if (str == "vi3") return altair::card::Sio::IntLevel::VI3;
	if (str == "vi4") return altair::card::Sio::IntLevel::VI4;
	if (str == "vi5") return altair::card::Sio::IntLevel::VI5;
	if (str == "vi6") return altair::card::Sio::IntLevel::VI6;
	if (str == "vi7") return altair::card::Sio::IntLevel::VI7;

	throw std::invalid_argument("Invalid interrupt level value!");
}


static std::string getImageFilePath(const std::string &baseDir, const std::string &image) {
	std::string ret = image;

	if (ret[0] != '/') {
		ret = baseDir + "/" + ret;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	altair::MainBoard board;

	signal(SIGINT, _signalHandler);

	common::Ini configuration;

	if (configuration.parse(argv[1])) {
		common::Ini::Section *global = nullptr;

		std::string configBaseDir = common::Utils::dirname(common::Utils::getRealPath(argv[1]));

		for (auto &i : configuration.getSections()) {
			if (i->getName() == "global") {
				global = i;

				continue;
			}

			if (i->getName() == "load") {
				continue;
			}

			if (i->getName() == "88-16mcs") {
				board.addCard(new altair::card::Mcs16(stringToSw1(i->getValue("bank"))));

				continue;
			}

			if (i->getName() == "88-iv") {
				board.addCard(new altair::card::VI(common::Utils::toUint8(i->getValue("port"))));

				continue;
			}

			if (i->getName() == "88-sio") {
				board.addCard(
					new altair::card::Sio(
						common::Utils::toUint8(i->getValue("port")),
						stringToSioBaud(i->getValue("baudrate")),
						stringToSioIntLevel(i->getValue("int-lvl-in")),
						stringToSioIntLevel(i->getValue("int-lvl-out"))
					)
				);

				continue;
			}

//			if (i->getName() == "88-2sio") {
//				board.addCard(
//					new altair::card::Sio2(
//						common::Utils::toUint8(i->getValue("port")),
//						stringToSioBaud(i->getValue("baudrate-a")),
//						stringToSioIntLevel(i->getValue("int-lvl-a")),
//						stringToSioBaud(i->getValue("baudrate-b")),
//						stringToSioIntLevel(i->getValue("int-lvl-b"))
//					)
//				);
//
//				continue;
//			}

			if (i->getName() == "88-pmc") {
				altair::card::Pmc *pmc = new altair::card::Pmc(
					common::Utils::toUint8(i->getValue("bank")),
					common::Utils::toUint8(i->getValue("wait"))
				);

				if (i->contains("file")) {
					auto loader = altair::utils::ImageLoaderFactory::getLoader(getImageFilePath(configBaseDir, i->getValue("file")));
					if (loader) {
						pmc->load(loader.get());
					}
				}

				board.addCard(pmc);
			}
		}

		// Load executables into memory
		if (configuration.contains("load")) {
			auto &loadables = configuration.get("load");

			if (! loadables.empty()) {
				std::string configBaseDir = common::Utils::dirname(common::Utils::getRealPath(argv[1]));

				std::unique_ptr<altair::card::DevelWriter> writer{new altair::card::DevelWriter()};

				board.addCard(writer.get());

				for (auto &l : loadables) {
					std::string loadablePath = getImageFilePath(configBaseDir, l->getValue("file"));

					DBG(("Loading: %s", loadablePath.c_str()));

					auto loader = altair::utils::ImageLoaderFactory::getLoader(loadablePath);

					writer->loadFrom(loader.get());
				}

				board.removeCard(writer.get());
			}
		}

		{
			uint16_t startPc = 0;

			if (global != nullptr) {
				if (global->contains("pc")) {
					startPc = common::Utils::toUint16(global->getValue("pc"));
				}

				if (global->contains("clk")) {
					altair::Config::setClkFrequency(
						common::Utils::toUint32(global->getValue("clk"))
					);
				}
			}

			altair::card::Cpu *cpuCard = new altair::card::Cpu(startPc);

			clk = cpuCard->getClock();

			board.addCard(cpuCard);
		}
	}

	try {
		clk->loop();
	} catch(...) {
		std::unique_ptr<altair::card::DevelReader> reader{new altair::card::DevelReader()};

		board.addCard(reader.get());
		{
			uint8_t buffer[0xffff]{};

			reader->read(0, buffer, 0xffff);

			dbg::dump(buffer, 0xffff);
		}
		board.removeCard(reader.get());
	}

	return 0;
}
