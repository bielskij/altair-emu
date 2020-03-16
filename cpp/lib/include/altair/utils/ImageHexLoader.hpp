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
#ifndef ALTAIR_UTILS_IMAGEHEXLOADER_HPP_
#define ALTAIR_UTILS_IMAGEHEXLOADER_HPP_

#include "common/Utils.hpp"
#include "altair/utils/ImageFileLoader.hpp"

namespace altair {
	namespace utils {
		class ImageHexLoader : public ImageFileLoader {
			public:
				ImageHexLoader() {
					this->line.reserve(11 + 510 + 1);

					this->lineOffset    = 0;
					this->addressOffset = 0;
				}

				bool nextByte(uint16_t &address, uint8_t &val) override {
					if (this->line.empty() || (this->lineOffset == this->line.size())) {
						this->line.clear();

						while (! this->isEof()) {
							uint8_t nextByte = this->ImageFileLoader::nextByte();

							if (nextByte == '\n') {
								this->line = common::Utils::trim(this->line, " \t\r\n");

								if (! this->line.empty()) {
									break;
								}
							}

							this->line.push_back(nextByte);
						}

						this->lineOffset = 0;

						// Validate read line
						if ((this->line.length() < 11) or (this->line[0] != ':')) {
							throw std::invalid_argument("Invalid ihex line! '" + this->line + "'");
						}

						// validate checksum
						{
							uint8_t checksumFile = common::Utils::hexToBin(this->line.c_str() + (this->line.size() - 2));
							uint8_t checksumCalc = 0;

							for (int i = 1; i < this->line.length() - 3; i += 2) {
								checksumCalc += common::Utils::hexToBin(this->line.c_str() + i);
							}
							checksumCalc = 0x100 - checksumCalc;

							if (checksumCalc != checksumFile) {
								throw std::invalid_argument("Invalid ihex line (bad crc)! '" + this->line + "'");
							}
						}

						uint8_t  recordType    = common::Utils::hexToBin(this->line.c_str() + 7);
						uint16_t recordAddress = (common::Utils::hexToBin(this->line.c_str() + 3) << 8) | common::Utils::hexToBin(this->line.c_str() + 5);

						// EOF record
						if (this->line.length() == 11 && (recordType != 0x01)) {
							throw std::invalid_argument("Invalid ihex line (bad eof)! '" + this->line + "'");
						}

						this->line = this->line.substr(9, this->line.length() - 11);

						// Parse address and type - it is not EOF record
						if (! this->line.empty()) {
							// Data record
							if (recordType != 0x00) {
								throw std::invalid_argument("Unsupported ihex record type! '" + this->line + "'");
							}

							this->addressOffset = recordAddress;
						}
					}

					if (! this->line.empty()) {
						address = this->addressOffset++;
						val     = common::Utils::hexToBin(this->line.c_str() + this->lineOffset);

						this->lineOffset += 2;

						return true;
					}

					return false;
				}

				void rewind() override {
					this->reset();

					this->line.clear();
					this->lineOffset    = 0;
					this->addressOffset = 0;
				}

				static bool probe(const std::string &path) {
					std::string extension = common::Utils::getFileExtension(path);

					if (extension == ".hex") {
						return true;
					}

					return false;
				}

			private:
				std::string line;
				int lineOffset;
				int addressOffset;
		};
	}
}

#endif /* ALTAIR_UTILS_IMAGEHEXLOADER_HPP_ */
