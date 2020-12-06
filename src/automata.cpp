/* automata.cpp
 */
#include "automata.hh"
#include <stdint.h>

Automata::Automata(size_t cellSize, size_t outputBit) :
	cells(0),
	cellSize(cellSize),
	cellMask((1 << cellSize) - 1), // count of rightmost bits set to 1 will equal to cellSize
	outputBit(outputBit)
{}

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
	cells = cells & cellMask; // Limit width of automata to cellSize

	// Returns next stateID
	return cells;
}

uint64_t Automata::getOutput()
{
	// Select and return nth rightmost bit
	uint64_t outputMask = 1 << outputBit;
	return (cells & outputMask) >> outputBit;
}
