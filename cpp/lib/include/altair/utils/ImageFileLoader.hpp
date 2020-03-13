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
#ifndef ALTAIR_UTILS_IMAGEFILELOADER_HPP_
#define ALTAIR_UTILS_IMAGEFILELOADER_HPP_

#include <fstream>
#include <iostream>

#include "altair/utils/ImageLoader.hpp"

namespace altair {
	namespace utils {
		class ImageFileLoader : public ImageLoader {
			public:
				~ImageFileLoader() {
					this->close();
				}

				void open(const std::string &filePath) {
					this->fileStream = std::ifstream(filePath, std::fstream::in | std::fstream::binary);
					if (this->fileStream.fail()) {
						throw std::runtime_error("Cannot open file: " + filePath);
					}
				}

				void close() {
					if (this->fileStream.is_open()) {
						this->fileStream.close();
					}
				}

			protected:
				uint8_t nextByte() {
					char ret;

					this->fileStream.read(&ret, 1);

					return ret;
				}

				bool isEof() {
					if (! this->fileStream.is_open()) {
						return true;
					}

					return this->fileStream.eof();
				}

				void reset() {
					if (this->fileStream.is_open()) {
						this->fileStream.seekg(0, std::ios_base::beg);
					}
				}

				size_t size() {
					size_t ret = 0;

					if (this->fileStream.is_open()) {
						std::streampos pos = this->fileStream.tellg();

						this->fileStream.seekg(0, std::ios_base::end);

						ret = this->fileStream.tellg();

						this->fileStream.seekg(pos, std::ios_base::beg);
					}

					return ret;
				}

			protected:
				ImageFileLoader() {
				}

				ImageFileLoader(const ImageFileLoader&) {
				}

			private:
				std::ifstream fileStream;
		};
	}
}



#endif /* ALTAIR_UTILS_IMAGEFILELOADER_HPP_ */
