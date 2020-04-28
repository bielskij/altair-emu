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
#ifndef ALTAIR_CARD_DEVEL_WRITER_CPP_
#define ALTAIR_CARD_DEVEL_WRITER_CPP_

#include "altair/utils/ImageLoader.hpp"

namespace altair {
	namespace card {
		class DevelWriter : public Card, public Connector {
			public:
				DevelWriter() {
				}

				card::Connector *getConnector() override {
					return this;
				}

				void loadFrom(altair::utils::ImageLoader *loader) {
					uint16_t address;
					uint8_t  data;

					loader->rewind();

					while (loader->nextByte(address, data)) {
						{
							this->psync(true);

							this->sinta(false);
							this->swo(true);
							this->sstack(false);
							this->shlta(false);
							this->sout(false);
							this->sm1(false);
							this->sinp(false);
							this->smemr(false);

							this->a(address);
						}
						this->clk();

						{
							this->psync(false);
						}
						this->clk();

						{
							this->dout(data);

							this->pwr(true);
						}
						this->clk();

						this->pwr(false);
					}
				}
		};
	}
}

#endif /* ALTAIR_CARD_DEVEL_WRITER_CPP_ */
