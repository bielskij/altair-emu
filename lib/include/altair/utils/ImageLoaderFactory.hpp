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
#ifndef ALTAIR_UTILS_IMAGELOADERFACTORY_HPP_
#define ALTAIR_UTILS_IMAGELOADERFACTORY_HPP_

#include <memory>

#include "altair/utils/ImageHexLoader.hpp"

namespace altair {
	namespace utils {
		class ImageLoaderFactory {
			public:
				static std::unique_ptr<ImageLoader> getLoader(const std::string &path) {
					if (ImageHexLoader::probe(path)) {
						auto ret = std::unique_ptr<ImageHexLoader>(new ImageHexLoader());

						ret->open(path);

						return ret;
					}

					return std::unique_ptr<ImageLoader>();
				}

			private:
				ImageLoaderFactory() = delete;
				ImageLoaderFactory(const ImageLoaderFactory&) = delete;
		};
	}
}


#endif /* ALTAIR_UTILS_IMAGELOADERFACTORY_HPP_ */
