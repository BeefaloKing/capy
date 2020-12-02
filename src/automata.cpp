/* automata.cpp
 */
#include "automata.hh"
#include <stdint.h>

Automata::Automata(size_t cellSize, size_t outputSize) :
	cells(0),
	cellSize(cellSize),
	outMask(new uint64_t[outputSize]),
	outputSize(outputSize)
{
	// TODO: Throw if outputSize > cellSize or cellSize > 64
	size_t spacing = cellSize / outputSize;
	size_t padding = cellSize - (spacing * outputSize - spacing + 1);
	size_t bitPos = padding / 2; // Padding is the space outside our evenly spaced output cells

	for (uint64_t i = 0; i < outputSize; i++)
	{
		outMask[i] = 1ull << bitPos;
		bitPos += spacing;
	}
}

Automata::~Automata()
{
	delete[] outMask;
}

void Automata::initState(uint64_t stateID)
{
	// stateID is equivalent to the bitmap of the states cells
	cells = stateID;
}

uint64_t Automata::advanceState()
{
	uint64_t left = cells >> 1; // Each position contains the bit originally on its left
	uint64_t right = cells << 1; // Each position contains the bit originally on its right

	cells = left ^ (cells | right); // Rule 30

	// Returns next stateID
	return cells;
}

uint64_t Automata::getOutput()
{
	uint64_t output = 0;

	uint64_t cursor = 1; // Set rightmost bit of cursor to 1
	for (size_t i = outputSize; i-- > 0;)
	{
		if (cells & outMask[i])
		{
			output |= cursor; // Set output bit at mask position to 1 if cell contains 1
		}
		cursor <<= 1; // Shift mask
	}

	return output;
}
