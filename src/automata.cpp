/* automata.cpp
 */
#include "automata.hh"
#include "error.hh"
#include <stdint.h>
#include <string>

Automata::Automata(size_t cellSize, size_t outputBit, uint8_t rule) :
	cells(0),
	cellSize(cellSize),
	cellMask((1 << cellSize) - 1), // count of rightmost bits set to 1 will equal to cellSize
	outputBit(outputBit),
	rule(rule)
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

	switch (rule)
	{
	case 30:
		cells = left ^ (cells | right);
		break;
	case 45:
		cells = ((left ^ cells) & right) | ~(left | right);
		break;
	case 54:
		cells = (left & ~cells) | (~left & (cells ^ right));
		break;
	case 90:
		cells = left ^ right;
		break;
	case 105:
		cells = (left & (cells ^ right)) | ~(left | (cells ^ right));
		break;
	case 124:
		cells = (left & ~(cells & right)) | (~left & cells);
		break;
	default:
		throwInvalidArg("Automata.rule", std::to_string(rule));
	}

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
