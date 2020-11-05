/* bitmap.cpp
 */
#include "bitmap.hh"
#include "error.hh"

BitMap::BitMap(uint64_t bitSize) :
	bitSize(bitSize)
{
	size_t arrayLen = (bitSize + 63) / 64; // Divide by number of bits in uint64_t and round up
	map = new uint64_t[arrayLen] {0}; // Explicitly initialize bitmap to all zeros
}

BitMap::~BitMap()
{
	delete[] map;
}

bool BitMap::get(uint64_t index)
{
	rangeCheck(index);

	size_t mapIndex = index / 64;
	uint64_t mask = 1ll << (index % 64); // Shift mask

	return map[mapIndex] & mask; // Extract single bit
}

void BitMap::set(uint64_t index, bool value)
{
	rangeCheck(index);

	size_t mapIndex = index / 64;
	uint64_t mask = 1ll << (index % 64); // Shift mask

	if (value)
	{
		map[mapIndex] |= mask; // Mask position is set 1
	}
	else
	{
		mask = ~mask; // Set all bits except mask position to 1 (binary not)
		map[mapIndex] &= mask; // All bits retain their value while mask position is set to 0
	}
}
