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
#include "altair/Core.hpp"


altair::Core::MachineCycle::MachineCycle(Core *core, bool inta, bool wo, bool stack, bool hlta, bool out, bool m1, bool inp, bool memr) {
	this->_core   = core;
	this->_status = 0;

	if (inta)  this->_status |= (1 << 0);
	if (wo)    this->_status |= (1 << 1);
	if (stack) this->_status |= (1 << 2);
	if (hlta)  this->_status |= (1 << 3);
	if (out)   this->_status |= (1 << 4);
	if (m1)    this->_status |= (1 << 5);
	if (inp)   this->_status |= (1 << 6);
	if (memr)  this->_status |= (1 << 7);
}


altair::Core::MachineCycle::~MachineCycle() {

};

bool altair::Core::MachineCycle::t1() {
	return false;
}

bool altair::Core::MachineCycle::t2() {
	return false;
}

bool altair::Core::MachineCycle::t3() {
	return false;
}

bool altair::Core::MachineCycle::t4() {
	return false;
}

bool altair::Core::MachineCycle::t5() {
	return false;
}

uint8_t altair::Core::MachineCycle::getStatus() {
	return this->_status;
}
