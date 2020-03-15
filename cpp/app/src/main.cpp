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

#include "altair/MainBoard.hpp"
#include "altair/Card/Cpu.hpp"
#include "altair/Card/Mcs16.hpp"
#include "altair/Card/Sio.hpp"

#include "altair/Card/devel/Writer.hpp"

#include "altair/utils/ImageLoaderFactory.hpp"


#define DEBUG_LEVEL 5
#include "common/debug.h"


static altair::card::Cpu::ClkSource *clk = nullptr;


static void _signalHandler(int signo) {
	clk->stop();
}


int main(int argc, char *argv[]) {
	altair::MainBoard board;

	signal(SIGINT, _signalHandler);

	{
		altair::card::Cpu *cpuCard = new altair::card::Cpu();

		clk = cpuCard->getClock();

		board.addCard(cpuCard);
	}

	// RAM
	board.addCard(new altair::card::Mcs16(altair::card::Mcs16::Sw1::COOO));
	board.addCard(new altair::card::Mcs16(altair::card::Mcs16::Sw1::OCOO));
	board.addCard(new altair::card::Mcs16(altair::card::Mcs16::Sw1::OOCO));

	board.addCard(new altair::card::Sio(0x00, altair::card::Sio::B19200));

	if (argc > 1) {
		auto loader = altair::utils::ImageLoaderFactory::getLoader(argv[1]);

		if (loader) {
			altair::card::DevelWriter *writer = new altair::card::DevelWriter();

			DBG(("Loading image from: %s", argv[1]));

			board.addCard(writer);

			writer->loadFrom(loader.get());
		}
	}

	clk->loop();

	return 0;
}
