#include <cstring>

#include "circularBuffer.h"

CircularBuffer::CircularBuffer(size_t pCapacity):
mBeginIndex(0),
mEndIndex(0),
mSize(0),
mCapacity(pCapacity)
{
	mData = new uint8_t[mCapacity];
}

CircularBuffer::~CircularBuffer()
{
	delete [] mData;
	mData = nullptr;
}

void CircularBuffer::clear()
{
	delete [] mData;
	mData = nullptr;

	mBeginIndex = 0;
	mEndIndex = 0;
	mSize = 0;

	mData = new uint8_t[mCapacity];
}

size_t CircularBuffer::write(uint8_t *data, size_t bytes)
{
	if(bytes == 0) {
		return 0;
	}

	size_t lCapacity = mCapacity;
	size_t lByteToWrite = std::min(bytes, lCapacity - mSize);

	// Write in a single step
	if(lByteToWrite <= lCapacity - mEndIndex) {
		memcpy(mData + mEndIndex, data, lByteToWrite);
		mEndIndex += lByteToWrite;

		if(mEndIndex == lCapacity) {
			mEndIndex = 0;
		}
	} else {	// Write in two steps
		size_t size_1 = lCapacity - mEndIndex;
		memcpy(mData + mEndIndex, data, size_1);

		size_t size_2 = lByteToWrite - size_1;
		memcpy(mData, data + size_1, size_2);

		mEndIndex = size_2;
	}

	mSize += lByteToWrite;

	return lByteToWrite;
}

size_t CircularBuffer::read(uint8_t *data, size_t bytes)
{
	if (bytes == 0) {
		return 0;
	}

	size_t lCapacity = mCapacity;
	size_t lByteToRead = std::min(bytes, mSize);

	// Read in a single step
	if (lByteToRead <= lCapacity - mBeginIndex) {
		memcpy(data, mData + mBeginIndex, lByteToRead);
		mBeginIndex += lByteToRead;

		if (mBeginIndex == lCapacity) {
			mBeginIndex = 0;
		}
	} else {	// Read in two steps
		size_t size_1 = lCapacity - mBeginIndex;
		memcpy(data, mData + mBeginIndex, size_1);
		size_t size_2 = lByteToRead - size_1;
		memcpy(data + size_1, mData, size_2);
		mBeginIndex = size_2;
	}

	mSize -= lByteToRead;

	return lByteToRead;
}

size_t CircularBuffer::preread(uint8_t *data, size_t bytes)
{
	if (bytes == 0) {
		return 0;
	}

	size_t lCapacity = mCapacity;
	size_t lByteToRead = std::min(bytes, mSize);

	// Read in a single step
	if (lByteToRead <= lCapacity - mBeginIndex) {
		memcpy(data, mData + mBeginIndex, lByteToRead);
	} else {	// Read in two steps
		size_t size_1 = lCapacity - mBeginIndex;
		memcpy(data, mData + mBeginIndex, size_1);
		size_t size_2 = lByteToRead - size_1;
		memcpy(data + size_1, mData, size_2);
	}

	return lByteToRead;
}


uint8_t *CircularBuffer::getBegBuff()
{
	if(mData == nullptr) {
		return nullptr;
	}

	return mData + mBeginIndex;
}

