/*******************************************************************************
 * Copyright (C) 2018 Jaroslaw Bielski (bielski.j@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include <stdlib.h>
#include "emulator/altair/module/88-mcs.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

/*
 * IO address: 0x08, 0x09, 0x0a
 * 71680 bytes per disk
 *  2048 bytes per track
 *   128 bytes per sector
 *
 *    16 sectors per track
 *    35 tracks per disk
 *
 */

#define SECTOR_SIZE      137 // Due to dsk format
#define SECTOS_PER_TRACK  16
#define TRACK_COUNT       35

#define STATE_CHANGE_INTERVAL 4

typedef enum _State {
	STATE_DISABLED,
	STATE_STARTING,
	STATE_IDLE,
	STATE_HEAD_MOVE
} State;

static _U8 sectorPosition = 0;
static _U8 trackPosition  = 0;
static _U8 bytePosition   = 0;

static State state = STATE_DISABLED;

static _U8 stateCounter = 0;
static _U8 byteWrRd     = 0;

FILE *diskFd = NULL;

#define _sleepMS(_x) usleep(_x * 1000)
#define _sleep() //_sleepMS(1)

static void _onTick(_U8 phase1, S100Bus *busState, void *privateData) {
	Altair88McsParameters *params = (Altair88McsParameters *) privateData;

	if (phase1) {
		_U8 address = busState->A >> 8;

		if (busState->SOUT && ! busState->_PWR) {
			_U8 data = busState->DO;

			switch (address) {
				case 0x08:
					// Enable one from 4 drives
					// D0, D1 - head is loaded when drive is enabled
					// D2 - d6 not used
					// D7 - 0 means enable drive
					DBG(("OUT ENABLE: data: %u", data));
					if ((data & 0x80) == 0) {
						if (state == STATE_DISABLED) {
							state = STATE_STARTING;

							trackPosition  = TRACK_COUNT - 1;
							sectorPosition = 0;
							bytePosition   = 0;
							stateCounter   = 0;
							byteWrRd       = 0;
						}

					} else {
						state = STATE_DISABLED;
					}
					break;

				case 0x09:
					{
#define CONTROL_STEP_IN      0x01
#define CONTROL_STEP_OUT     0x02
#define CONTROL_TIMER_RESET  0x04
#define CONTROL_INT_ENABLE   0x10
#define CONTROL_INT_DISABLE  0x20
#define CONTROL_WRITE_ENABLE 0x80

						// controls drive operation
						DBG(("OUT CONTROL %02x", data));

						if (state == STATE_IDLE) {
							if (data & CONTROL_STEP_IN) {
								trackPosition++;
								DBG(("Track position: %u", trackPosition));

								stateCounter = 0;
								state = STATE_HEAD_MOVE;

							} else if (data & CONTROL_STEP_OUT) {
								trackPosition--;
								DBG(("Track position: %u", trackPosition));

								stateCounter = 0;
								state = STATE_HEAD_MOVE;
							}

							_sleep();
						}
					}
					break;

				case 0x0a:
					// write data
					DBG(("OUT WRITE"));
					break;
			}

		} else if (busState->SINP && busState->PDBIN) {
#define STATUS_ENWD  0x01
#define STATUS_MH    0x02
#define STATUS_HS    0x04
#define STATUS_INTE  0x20
#define STATUS_TRACK 0x40
#define STATUS_NRDA  0x80

			switch (address) {
				case 0x08:
					// status of drives and controllers
//					DBG(("IN status, state: %u", state));

					if (state != STATE_DISABLED) {
						// 0x18 always false bits when drive is enabled
						busState->DI = 0x18;
					} else {
						busState->DI = 0x00;
					}

					switch (state) {
						case STATE_DISABLED:
							break;

						case STATE_STARTING:
							state = STATE_IDLE;
							break;

						case STATE_IDLE:
							busState->DI |= (STATUS_MH | STATUS_HS);

							if (trackPosition == 0) {
								busState->DI |= STATUS_TRACK;
							}

							// TODO: write mode
							if (! byteWrRd) {
								busState->DI |= STATUS_NRDA;
							}

							break;

						case STATE_HEAD_MOVE:
							busState->DI |= (STATUS_HS);
							if (trackPosition == 0) {
								busState->DI |= STATUS_TRACK;
							}

							stateCounter = 0;
							byteWrRd     = 0;

							state = STATE_IDLE;
							break;
					}

					busState->DI = ~busState->DI;

//					DBG(("STATUS RET: %02x, track: %u, sector: %u", busState->DI, trackPosition, sectorPosition));

					_sleep();

					break;

				case 0x09:
					// sector position of diskette
					DBG(("SECTOR POS track: %u, sector: %u, byte: %u", trackPosition, sectorPosition, bytePosition));

					busState->DI = (sectorPosition << 1) | (bytePosition > 64);

					_sleep();
					break;

				case 0x0a:
					DBG(("IN data, track: %u, sector: %u, byte: %u, f: %p",
						trackPosition, sectorPosition, bytePosition, diskFd
					));

					byteWrRd = 1;

					{
						_U8 data;

						if (diskFd) {
							_U32 offset = trackPosition * SECTOS_PER_TRACK * SECTOR_SIZE;

							offset += (sectorPosition * SECTOR_SIZE);
							offset += (bytePosition);

							DBG(("READ from file at %u", offset));
							fseek(diskFd, offset, SEEK_SET);
							fread(&data, 1, 1, diskFd);
						}

						busState->DI = data;
					}

					_sleep();
					break;
			}

			if (state == STATE_IDLE) {
				stateCounter++;
				if (stateCounter == STATE_CHANGE_INTERVAL) {
					stateCounter = 0;
					byteWrRd    = 0;

					bytePosition++;
					if (bytePosition == SECTOR_SIZE) {
						bytePosition = 0;

						sectorPosition++;
						if (sectorPosition == 16) {
							sectorPosition = 0;
						}
					}
				}

			} else if (state == STATE_HEAD_MOVE) {
				stateCounter++;

				if (stateCounter == STATE_CHANGE_INTERVAL) {
					stateCounter = 0;

					state = STATE_IDLE;
				}

			} else if (state == STATE_STARTING) {
				stateCounter++;

				if (stateCounter == STATE_CHANGE_INTERVAL) {
					stateCounter = 0;

					state = STATE_IDLE;
				}
			}
		}
	}
}


void altair_module_88mcs_init(AltairModule *module, Altair88McsParameters *params) {
	module->privateData   = params;
	module->clockCallback = _onTick;

	{
		char *envVal = getenv("MD_FILE");
		if (envVal != NULL) {
			DBG(("Opening disk at: %s", envVal));

			diskFd = fopen(envVal, "r");
		}
	}
}
