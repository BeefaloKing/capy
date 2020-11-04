/* automata.cpp
 */
#include "automata.hh"
#include <stdint.h>

Automata::Automata(size_t cellSize, size_t outputSize) :
	cells(new uint8_t[cellSize]),
	cellSwap(new uint8_t[cellSize]),
	cellSize(cellSize),
	outIndex(new size_t[outputSize]),
	outputSize(outputSize)
{
	// TODO: Throw if outputSize > cellSize or cellSize > 64
	size_t spacing = cellSize / outputSize;
	size_t padding = cellSize - (spacing * outputSize - spacing + 1);
	size_t index = padding / 2; // Padding is the space outside our evenly spaced output cells
	for (size_t i = 0; i < outputSize; i++)
	{
		outIndex[i] = index;
		index += spacing;
	}
}

Automata::~Automata()
{
	delete[] cells;
	delete[] cellSwap;
	delete[] outIndex;
}

void Automata::initState(uint64_t stateID)
{
	// stateID is equivalent to the bitmap of the states cells
	for (size_t i = cellSize; i-- > 0;)
	{
		cells[i] = stateID & 1; // Extract rightmost bit of stateID
		stateID >>= 1;
	}
}

void Automata::advanceState()
{
	for (size_t i = 0; i < cellSize; i++)
	{
		// Default to 0 for cells out of bounds
		uint8_t left = (i == 0) ? 0 : cells[i - 1];
		uint8_t right = (i == cellSize - 1) ? 0 : cells[i + 1];
		uint8_t center = cells[i];

		cellSwap[i] = left ^ (center | right); // Rule 30
	}

	// Swap out "framebuffer"
	uint8_t* temp = cells;
	cells = cellSwap;
	cellSwap = temp;
}

uint64_t Automata::getOutput()
{
	uint64_t output = 0;

	uint64_t mask = 1; // Set rightmost bit of mask to 1
	for (size_t i = outputSize; i-- > 0;)
	{
		size_t cellIndex = outIndex[i];
		if (cells[cellIndex])
		{
			output |= mask; // Set output bit at mask position to 1 if cell contains 1
		}
		mask <<= 1; // Shift mask
	}

	return output;
}
