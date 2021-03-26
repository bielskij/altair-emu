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
#ifndef ALTAIR_CARD_88_DCDD_HPP_
#define ALTAIR_CARD_88_DCDD_HPP_

#include <algorithm>
#include <map>
#include <fstream>

#include "altair/MainBoard.hpp"
#include "altair/utils/ImageLoader.hpp"

namespace altair {
	namespace card {
		/*!
		 * 88-DCDD 8" floppy disk drive
		 */
		class Dcdd : public Card, protected Connector {
			private:
				static constexpr uint32_t TRACK_COUNT   = 77;
				static constexpr uint32_t TRACK_SECTORS = 32;
				static constexpr uint32_t SECTOR_SIZE   = 137;

			public:
				class Fd400 {
					private:
						enum State {
							UNLOADED,
							LOADING,
							LOADED,
							UNLOADING,
							STEP_IN,
							STEP_OUT,
						};

						static constexpr uint8_t CTRL_STEP_IN  = 0x01;
						static constexpr uint8_t CTRL_STEP_OUT = 0x02;
						static constexpr uint8_t CTRL_HL       = 0x04; // loads head onto disk
						static constexpr uint8_t CTRL_HUL      = 0x08; // removes head from disk surface
						static constexpr uint8_t CTRL_IE       = 0x10; // Interrupt enable
						static constexpr uint8_t CTRL_ID       = 0x20; // Interrupt disable
						static constexpr uint8_t CTRL_HCS      = 0x40; // Head current switch
						static constexpr uint8_t CTRL_WE       = 0x80; // Write enable

						static constexpr uint8_t STATUS_ENWD   = 0x01; // Enter new write data - indicates write circuit is ready for new data
						static constexpr uint8_t STATUS_MH     = 0x02; // Move Head - indicates head movement allowed when true (step in/out)
						static constexpr uint8_t STATUS_HS     = 0x04; // True 40ms after head loaded or step command
						static constexpr uint8_t STATUS_INTE   = 0x20; // Indicates interrupt enabled
						static constexpr uint8_t STATUS_TRACK0 = 0x40;
						static constexpr uint8_t STATUS_NRDA   = 0x80; // New read data available

					public:
						Fd400(const std::string &filePath, uint8_t address);
						~Fd400();

						uint8_t getAddress() const {
							return this->_address;
						}

						uint8_t getStatus();
						uint8_t getSectorPosition();
						uint8_t getByte();
						void    cmd(uint8_t cmd);
						void    enable(bool enable);

						void tick();
						void tick(uint8_t ticks);

					private:
						static uint32_t getTicksFromMs(uint32_t ms);
						static uint32_t getTicksFromUs(uint32_t us);
						void reset();
						void bufferTrack(uint8_t trackNo);

					private:
						uint8_t      _address;
						std::string  _filePath;
						State        _state;
						std::fstream _file;

						uint8_t  _track;
						uint8_t  _sector;
						uint8_t  _sectorByte;
						bool     _enabled;

						uint32_t _transitionTicks; // Remaining ticks required by transition (load/unload or step)

						uint32_t _moveHeadTicks;
						uint32_t _moveHeadFalseTicks;
						uint32_t _moveHeadTotalTicks;

						uint32_t _sectorTicks;
						uint32_t _sectorDelayTicks;
						uint32_t _sectorTrueTicks;
						uint32_t _sectorTotalTicks;

						uint32_t _sectorByteTrueTicks;
						uint32_t _sectorByteTotalTicks;
						bool     _sectorByteRead;

						uint8_t _trackBuffer[TRACK_SECTORS * SECTOR_SIZE];
				};

			public:
				Dcdd() {
					this->_addr       = 0;
					this->_intEnabled = false;
				}

				card::Connector *getConnector() override {
					return this;
				}

				void addDrive(Fd400 *drive) {
					this->_drives[drive->getAddress()] = drive;
				}

			protected:
				// Connector methods
				bool onMemoryRead(uint16_t address, uint8_t &val) override {
					return false;
				}

				void onMemoryWrite(uint16_t address, uint8_t data) override {
				}

				bool onIn(uint8_t number, uint8_t &val) override;
				void onOut(uint8_t number, uint8_t data) override;
				void onClk() override;
				void onClk(uint8_t ticks) override;

			private:
				uint8_t                    _addr;
				std::map<uint8_t, Fd400 *> _drives;
				bool                       _intEnabled;
		};
	}
}

#endif /* ALTAIR_CARD_88_DCDD_HPP_ */
