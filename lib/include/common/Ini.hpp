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
#ifndef COMMON_INI_HPP_
#define COMMON_INI_HPP_

#include <map>
#include <vector>
#include <algorithm>


namespace common {
	class Ini {
		public:
			class Section {
				public:
					Section(const std::string &sectionName) : _sectionName(sectionName) {
					}

					virtual ~Section() {

					}

					bool contains(const std::string &key) const {
						return this->_entries.find(key) != this->_entries.end();
					}

					const std::string &getValue(const std::string &key) const {
						if (this->contains(key)) {
							return this->_entries.at(key);
						}

						throw std::invalid_argument("Key '" + key + "' was not found!");
					}

					const std::string &getName() const {
						return this->_sectionName;
					}

					void add(const std::string &key, const std::string &val) {
						this->_entries[key] = val;
					}

					std::vector<std::string> getKeys() const {
						std::vector<std::string> ret;

						for (auto &i : _entries) {
							ret.push_back(i.first);
						}

						return ret;
					}

				private:
					std::string _sectionName;
					std::map<std::string, std::string> _entries;
			};

		public:
			Ini() {
			}

			virtual ~Ini() {
			}

			bool parse(const std::string &path);

			bool contains(const std::string &sectionName) const {
				return this->_sectionMap.find(sectionName) != this->_sectionMap.end();
			}

			const std::vector<Section *> &get(const std::string &sectionName) const {
				if (this->contains(sectionName)) {
					return this->_sectionMap.at(sectionName);
				}

				throw std::invalid_argument("Section '" + sectionName + "' was not found!");
			}

			std::vector<Section *> getSections() const {
				std::vector<Section *> ret;

				for (auto &i : this->_sectionMap) {
					std::copy(i.second.begin(), i.second.end(), std::back_inserter(ret));
				}

				return ret;
			}

		private:
			std::map<std::string, std::vector<Section *>> _sectionMap;
	};
}


#endif /* COMMON_INI_HPP_ */
