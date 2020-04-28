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
#include <cinttypes>

#include "altair/Card/Dcdd.hpp"
#include "altair/Config.hpp"

#define DEBUG_LEVEL 1
#include "common/debug.h"


altair::card::Dcdd::Fd400::Fd400(const std::string &filePath, uint8_t address) : _filePath(filePath) {
	this->_address = address;
	this->_state   = State::UNLOADED;
	this->_enabled = false;

	this->_sector     = 0;
	this->_track      = 0;
	this->_sectorByte = 0;
	this->_transitionTicks   = 0;

	this->_moveHeadFalseTicks = 0;
	this->_moveHeadTicks      = 0;
	this->_moveHeadTotalTicks = 0;

	this->_sectorTicks      = 0;
	this->_sectorTotalTicks = 0;
	this->_sectorTrueTicks  = 0;

	this->_sectorDelayTicks     = 0;
	this->_sectorByteTotalTicks = 0;
	this->_sectorByteTrueTicks  = 0;
	this->_sectorByteRead       = false;

	this->_file.open(filePath, std::ios::in | std::ios::binary);
}


altair::card::Dcdd::Fd400::~Fd400() {
	if (this->_file.is_open()) {
		this->_file.close();
	}
}


void altair::card::Dcdd::Fd400::reset() {

}


void altair::card::Dcdd::Fd400::bufferTrack(uint8_t trackNo) {
	if (this->_file.is_open()) {
		this->_file.seekp(this->_track * TRACK_SECTORS * SECTOR_SIZE, std::ios_base::beg);

		this->_file.read(reinterpret_cast<char *>(this->_trackBuffer), TRACK_SECTORS * SECTOR_SIZE);
	}
}


uint32_t altair::card::Dcdd::Fd400::getTicksFromMs(uint32_t ms) {
	uint32_t nsPerTick = 1000000000ULL / altair::Config::getClkFrequency();

	return (ms * 1000000) / nsPerTick;
}


uint32_t altair::card::Dcdd::Fd400::getTicksFromUs(uint32_t us) {
	uint32_t nsPerTick = 1000000000ULL / altair::Config::getClkFrequency();

	return (us * 1000) / nsPerTick;
}


void altair::card::Dcdd::Fd400::tick() {
	if (this->_state == State::LOADED) {
		if (this->_moveHeadTicks == this->_moveHeadTotalTicks) {
			this->_moveHeadTicks = 0;
		}
		this->_moveHeadTicks++;

		// Whole sector read
		if (this->_sectorTicks == this->_sectorTotalTicks) {
			this->_sectorByteTotalTicks = 0;
			this->_sectorByteTrueTicks  = 0;
			this->_sectorTicks          = 0;
			this->_sectorByte           = 0;
			this->_sectorByteRead       = false;

			this->_sector++;
			if (this->_sector == TRACK_SECTORS) {
				this->_sector = 0;
			}

		// Wait 280uS delay
		} else if (this->_sectorTicks == this->_sectorDelayTicks) {
			this->_sectorByteTrueTicks  = this->_sectorTicks + getTicksFromUs(16); // shorter time causes problems with double byte read
			this->_sectorByteTotalTicks = this->_sectorTicks + getTicksFromUs(32);

		} else if (this->_sectorTicks > this->_sectorDelayTicks) {
			if (this->_sectorTicks == this->_sectorByteTotalTicks) {
				// Sector byte
				if (this->_sectorByte < SECTOR_SIZE) {
					this->_sectorByteTrueTicks  = this->_sectorTicks + getTicksFromUs(16);
					this->_sectorByteTotalTicks = this->_sectorTicks + getTicksFromUs(32);

					this->_sectorByte++;
					this->_sectorByteRead = false;
				}
			}
		}
		this->_sectorTicks++;
	}

	if (this->_transitionTicks > 0) {
		this->_transitionTicks--;

		if (this->_transitionTicks == 0) {
			switch (this->_state) {
				case State::LOADING:
					{
						DBG(("LOADING -> LOADED"));

						this->_state = State::LOADED;

						this->_track         = 0;
						this->_sector        = 0;

						this->_moveHeadTicks = 0;
						this->_sectorTicks   = 0;

						this->bufferTrack(this->_track);
					}
					break;

				case State::UNLOADING:
					{
						DBG(("UNLOADING -> UNLOADED"));

						this->_state = State::UNLOADED;
					}
					break;

				case State::STEP_IN:
					{
						this->_track++;
						this->_state = State::LOADED;

						this->_moveHeadTicks = 0;

						this->_sectorTicks          = 0;
						this->_sectorByte           = 0;
						this->_sectorByteTotalTicks = 0;
						this->_sectorByteTrueTicks  = 0;
						this->_sectorByteRead       = false;

						this->bufferTrack(this->_track);

						DBG(("STEP_IN(%u) -> LOADED", this->_track));
					}
					break;

				case State::STEP_OUT:
					{
						this->_track--;
						this->_state = State::LOADED;

						this->_moveHeadTicks = 0;
						this->_sectorTicks   = 0;

						this->_sectorByte           = 0;
						this->_sectorByteTotalTicks = 0;
						this->_sectorByteTrueTicks  = 0;
						this->_sectorByteRead       = false;

						this->bufferTrack(this->_track);

						DBG(("STEP_OUT(%u) -> LOADED", this->_track));
					}
					break;

				default:
					break;
			}
		}
	}
}


uint8_t altair::card::Dcdd::Fd400::getStatus() {
	if (this->_file.good() && this->_enabled) {
		uint8_t ret = 0xe7; // 3 and 4 bit should be 0 when enabled

		if (this->_state == State::LOADED || this->_state == State::LOADING) {
			if (this->_track == 0) {
				ret &= ~STATUS_TRACK0;
			}

			if (this->_state == State::LOADED) {
				ret &= ~STATUS_HS;

				if (this->_moveHeadTicks < this->_moveHeadFalseTicks) {
					ret &= ~STATUS_MH;
				}

				if (this->_sectorTicks > this->_sectorDelayTicks) {
					if ((this->_sectorTicks < this->_sectorByteTrueTicks) && ! this->_sectorByteRead) {
						DBG(("NRDA"));

						ret &= ~STATUS_NRDA;
					}
				}
			}
		}

		return ret;
	}

	return 0xff;
}


uint8_t altair::card::Dcdd::Fd400::getSectorPosition() {
	if (this->_file.good() && this->_enabled) {
		uint8_t ret = 0;

		ret |= (this->_sector << 1);

		if (this->_sectorTicks > this->_sectorTrueTicks) {
			ret |= 0x01;
		}

//		DBG(("[%u] Sector pos: %u (true: %u)", this->_address, ret >> 1, ret & 1));

		return ret;
	}

	return 0x01;
}


uint8_t altair::card::Dcdd::Fd400::getByte() {
	if (this->_file.good() && this->_enabled) {
		this->_sectorByteRead = true;

		return this->_trackBuffer[this->_sector * SECTOR_SIZE + this->_sectorByte];
	}

	return 0;
}


void altair::card::Dcdd::Fd400::cmd(uint8_t cmd) {
	switch (this->_state) {
		case State::UNLOADED:
			if (cmd & CTRL_HL) {
				DBG(("HEAD LOAD"));

				this->_state = State::LOADING;

				this->_transitionTicks = getTicksFromMs(40);
			}
			break;

		case State::LOADED:
			{
				if (cmd & CTRL_HUL) {
					DBG(("HEAD UNLOAD"));

					this->_state           = State::UNLOADING;
					this->_transitionTicks = getTicksFromMs(40);
				}

				if (cmd & CTRL_STEP_IN) {
					DBG(("STEP IN"));

					if (this->_track < TRACK_COUNT - 1) {
						this->_state           = State::STEP_IN;
						this->_transitionTicks = getTicksFromMs(20);
					}
				}

				if (cmd & CTRL_STEP_OUT) {
					DBG(("STEP OUT"));

					if (this->_track > 0) {
						this->_state           = State::STEP_OUT;
						this->_transitionTicks = getTicksFromMs(20);
					}
				}
			}
			break;

		default:
			break;
	}

	if (cmd & CTRL_IE) {
		DBG(("INT ENABLE"));
	}

	if (cmd & CTRL_ID) {
		DBG(("INT DISABLE"));
	}

	if (cmd & CTRL_HCS) {
		DBG(("HEAD CURRENT"));
	}

	if (cmd & CTRL_WE) {
		DBG(("WRITE ENABLE"));
	}
}


void altair::card::Dcdd::Fd400::enable(bool enable) {
	this->_enabled = enable;

	if (this->_enabled) {
		this->_moveHeadFalseTicks = getTicksFromMs(1);
		this->_moveHeadTotalTicks = getTicksFromMs(10);

		this->_sectorDelayTicks = getTicksFromUs(280);
		this->_sectorTrueTicks  = getTicksFromUs(30);
		this->_sectorTotalTicks = getTicksFromUs(5200);
	}
}


void altair::card::Dcdd::onClk() {
	for (auto &p : this->_drives) {
		if (p.second != nullptr) {
			p.second->tick();
		}
	}

	this->SimpleConnector::onClk();
}


bool altair::card::Dcdd::onIn(uint8_t number, uint8_t &val) {
	bool ret = true;

	switch (number) {
		case 0x08:
			{
				Fd400 *drive = this->_drives[this->_addr];
				if (drive == nullptr) {
					val = 0xff;

				} else {
					val = drive->getStatus();
				}

				DBG(("[%u] STATUS %02x", this->_addr, val));
			}
			break;

		case 0x09:
			{
				Fd400 *drive = this->_drives[this->_addr];
				if (drive == nullptr) {
					val = 0x01;

				} else {
					val = drive->getSectorPosition();
				}

//				DBG(("[%u] SECTOR: %02x", this->_addr, val));
			}
			break;

		case 0x0a:
			{
				Fd400 *drive = this->_drives[this->_addr];
				if (drive == nullptr) {
					val = 0x00;

				} else {
					val = drive->getByte();
				}
			}
			break;

		default:
			ret = false;
			break;
	}

	return ret;
}


void altair::card::Dcdd::onOut(uint8_t number, uint8_t data) {
	switch (number) {
		case 0x08:
			{
				Fd400 *drive = this->_drives[this->_addr];
				if (drive != nullptr) {
					drive->enable(false);
				}

				// TODO: Handle clear control flag
				this->_addr = data & 0x0f;

				DBG(("SELECTION %u", this->_addr));

				drive = this->_drives[this->_addr];
				if (drive != nullptr) {
					drive->enable(true);
				}
			}
			break;

		case 0x09:
			{
				Fd400 *drive = this->_drives[this->_addr];

				DBG(("[%u] CONTROL", this->_addr));

				if (drive == nullptr) {
					ERR(("Drive is null!"));

				} else {
					drive->cmd(data);
				}
			}
			break;

		case 0x0a:
			{

			}
			break;
	}
}
