/*
 * main.cpp
 *
 *  Created on: 28.02.2020
 *      Author: jarko
 */

#include "altair/Cpu.hpp"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class PioImpl : public altair::Cpu::Pio {
	public:
		uint16_t address;
		uint8_t  data;

		bool sync;
		bool dbin;
		bool wr;

	public:
		virtual bool getInt() {
			DBG(("CALL"));

			return false;
		}

		virtual bool getHold() {
			DBG(("CALL"));

			return false;
		}

		virtual bool getReady() {
			DBG(("CALL"));

			return true;
		}

		virtual bool getReset() {
			DBG(("CALL"));

			return false;
		}

		virtual void setWr(bool active) {
			DBG(("CALL"));
		}

		virtual void setDbin(bool active) {
			DBG(("CALL"));

			this->dbin = active;
		}

		virtual void setInte(bool active) {
			DBG(("CALL"));
		}

		virtual void setHoldAck(bool active) {
			DBG(("CALL"));
		}

		virtual void setWait(bool active) {
			DBG(("CALL"));
		}

		virtual void setSync(bool active) {
			DBG(("CALL"));
		}

		virtual uint8_t getData() {
			DBG(("CALL, dbin: %d, address: %04x", this->dbin, this->address));

			if (this->dbin) {
				return 0x6b;
			}

			return 0xff;
		}

		virtual void    setData(uint8_t val) {
			this->data = val;
		}

		virtual void setAddress(uint16_t val) {
			this->address = val;
		}

		virtual void clk() {
			DBG(("CALL"));
		};
};


int main(int argc, char *argv[]) {
	PioImpl pio;

	altair::Cpu cpu(pio);

	for (int i = 0; i < 20; i++) {
		cpu.tick();
		cpu.tick();
		cpu.tick();
	}
}
