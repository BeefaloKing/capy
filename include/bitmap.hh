/* bitmap.hh
 * Used to efficiently store boolean data
 */
#pragma once
#include <stdint.h>
#include "error.hh"

class BitMap
{
public:
	BitMap() = delete; // Does not support dynamic reallocation
	BitMap(uint64_t bitSize); // size_t may not be large enough on 32 bit architecture
	~BitMap();

	bool get(uint64_t index);
	void set(uint64_t index, bool value);
private:
	inline void rangeCheck(uint64_t index)
	{
		if (index >= bitSize)
		{
			throwRange("BitMap", index, bitSize);
		}
	}

	uint64_t* map;
	uint64_t bitSize;
};
