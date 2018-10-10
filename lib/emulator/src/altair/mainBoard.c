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

#include "emulator/altair/mainBoard.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

typedef struct _ModuleListItem {
	AltairModule           *module;
	struct _ModuleListItem *next;
} ModuleListItem;

static ModuleListItem _listItems[ALTAIR_MODULES_COUNT * 5];
static _U8            _listItemsUsed = 0;

static ModuleListItem *_listClock = NULL;
static ModuleListItem *_listMemRd = NULL;
static ModuleListItem *_listMemWr = NULL;
static ModuleListItem *_listIoRd  = NULL;
static ModuleListItem *_listIoWr  = NULL;

static S100Bus _bus;


static ModuleListItem *_getNextItem(AltairModule *module) {
	ModuleListItem *ret = &_listItems[_listItemsUsed++];

	ret->module = module;
	ret->next   = NULL;

	return ret;
}


static void _addNextItem(ModuleListItem **list, ModuleListItem *node) {
	node->next = *list;

	*list = node;
}


void altair_mainBoard_initialize() {
	_bus._PINT = 1;
}


void altair_mainBoard_addModule(AltairModule *module) {
	if (module->clockCallback) {
		_addNextItem(&_listClock, _getNextItem(module));
	}

	if (module->readIoCallback) {
		_addNextItem(&_listIoRd, _getNextItem(module));
	}

	if (module->writeIoCallback) {
		_addNextItem(&_listIoWr, _getNextItem(module));
	}

	if (module->readMemoryCallback) {
		_addNextItem(&_listMemRd, _getNextItem(module));
	}

	if (module->writeMemoryCallback) {
		_addNextItem(&_listMemWr, _getNextItem(module));
	}
}


void altair_mainBoard_tick() {
	ModuleListItem *i;

	// phase1
	i = _listClock;
	while (i) {
		i->module->clockCallback(TRUE, &_bus, i->module->privateData);

		i = i->next;
	}

	if (_bus.PDBIN) {
		// Memory read
		if (_bus.SMEMR) {
			i = _listMemRd;
			while (i) {
				i->module->readMemoryCallback(&_bus, i->module->privateData);

				i = i->next;
			}

		// IO read
		} else if (_bus.SINP) {
			i = _listIoRd;
			while (i) {
				i->module->readIoCallback(&_bus, i->module->privateData);

				i = i->next;
			}
		}

	// Memory write
	} else if (_bus.MWRT) {
		i = _listMemWr;
		while (i) {
			i->module->writeMemoryCallback(&_bus, i->module->privateData);

			i = i->next;
		}

	// IO write
	} else if (_bus.SOUT && _bus._PWR) {
		i = _listIoWr;
		while (i) {
			i->module->writeIoCallback(&_bus, i->module->privateData);

			i = i->next;
		}
	}

	// phase2
	i = _listClock;
	while (i) {
		i->module->clockCallback(FALSE, &_bus, i->module->privateData);

		i = i->next;
	}
}


void altair_mainBoard_ticks(_U32 ticksCount) {
	for (_U32 i = 0; i < ticksCount; i++) {
		altair_mainBoard_tick();
	}
}
