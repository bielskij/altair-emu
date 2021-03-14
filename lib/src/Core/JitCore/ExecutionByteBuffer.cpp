/*
 * ExecutionByteBuffer.cpp
 *
 *  Created on: 14.03.2021
 *      Author: Jaroslaw Bielski (bielski.j@gmail.com)
 */

#include <sys/mman.h>
#include <unistd.h>

#include "altair/Core/JitCore.hpp"


altair::JitCore::ExecutionByteBuffer::ExecutionByteBuffer() : ExecutionByteBuffer(0) {
}


altair::JitCore::ExecutionByteBuffer::ExecutionByteBuffer(size_t initialSize) {
	this->_pageSize   = sysconf(_SC_PAGESIZE);
	this->_bufferSize = (initialSize / this->_pageSize) * this->_pageSize;

	if (this->_bufferSize < initialSize) {
		this->_bufferSize += this->_pageSize;

	} else if (this->_bufferSize == 0) {
		this->_bufferSize = this->_pageSize;
	}

	this->_buffer        = (uint8_t *) mmap(nullptr, this->_bufferSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	this->_bufferWritten = 0;
}


altair::JitCore::ExecutionByteBuffer::~ExecutionByteBuffer() {
	munmap(this->_buffer, this->_bufferSize);
}


altair::JitCore::ExecutionByteBuffer &altair::JitCore::ExecutionByteBuffer::begin() {
	return *this;
}


altair::JitCore::ExecutionByteBuffer &altair::JitCore::ExecutionByteBuffer::appendByte(uint8_t byte) {
	this->_buffer[this->_bufferWritten++] = byte;

	return *this;
}


altair::JitCore::ExecutionByteBuffer &altair::JitCore::ExecutionByteBuffer::end() {
	mprotect(this->_buffer, this->_bufferSize, PROT_EXEC);

	return *this;
}


altair::JitCore::ExecutionByteBuffer::FunctionPtr altair::JitCore::ExecutionByteBuffer::getFunction() {
	return reinterpret_cast<FunctionPtr>(this->_buffer);
}


void *altair::JitCore::ExecutionByteBuffer::getPtr() {
	return this->_buffer;
}
