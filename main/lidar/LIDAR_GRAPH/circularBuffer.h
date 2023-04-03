#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>

#include <algorithm> // for std::min

class CircularBuffer
{
public:
	CircularBuffer(size_t pCapacity);
	~CircularBuffer();

	size_t size() const { return mSize; }
	size_t capacity() const { return mCapacity; }
	// Return number of bytes written.
	size_t write(uint8_t *data, size_t bytes);
	// Return number of bytes read.
	size_t read(uint8_t *data, size_t bytes);
	uint8_t *getBegBuff();
	void clear();
	size_t preread(uint8_t *data, size_t bytes);

private:
	size_t mBeginIndex;
	size_t mEndIndex;
	size_t mSize;
	size_t mCapacity;

	uint8_t *mData;
};

#endif //__RING_BUFFER_H
