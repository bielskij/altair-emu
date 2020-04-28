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

#include <fstream>

#include "common/Ini.hpp"
#include "common/Utils.hpp"


#define VALUE_LENGTH_MAX 255


bool common::Ini::parse(const std::string &path) {
	bool ret = true;

	{
		std::ifstream ifs;

		ifs.open(path.c_str(), std::ifstream::in);

		ret = ifs.is_open();
		if (ret) {
			std::string line;

			bool parsingKey = true;
			bool isString = false;

			Section *section = nullptr;

			while (std::getline(ifs, line)) {
				line = Utils::trim(line, " \t\r\n");

				if (line.empty()) {
					continue;
				}

				// Section header
				if (*line.begin() == '[' && *line.rbegin() == ']') {
					// section
					if (section != nullptr) {
						this->_sectionMap[section->getName()].push_back(section);
					}

					section = new Section(line.substr(1, line.length() - 2));

					continue;

				// Skip comments
				} else if (*line.begin() == ';' || *line.begin() == '#') {
					continue;

				// key = value
				} else {
					std::string key;
					std::string val;

					size_t pos = line.find_last_of('=');
					if (pos == std::string::npos) {
						throw std::invalid_argument("Invalid line! '" + line + "'");
					}

					key = Utils::trim(line.substr(0, pos), " \t");
					val = Utils::trim(line.substr(pos + 1), " \t");

					section->add(key, val);
				}
			}

			if (section != nullptr) {
				this->_sectionMap[section->getName()].push_back(section);
			}
		}
	}

	return ret;
}
