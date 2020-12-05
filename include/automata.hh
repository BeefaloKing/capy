/* automata.hh
 */
#pragma once
#include <stdint.h>

class Automata
{
public:
	Automata() = delete;
	Automata(size_t cellSize, size_t outputSize = 1); // Multibit output is deprecated
	~Automata();

	void initState(uint64_t stateID);

	uint64_t advanceState(); // Returns the next stateID
	uint64_t getOutput();
private:
	uint64_t cells;
	size_t cellSize;
	uint64_t* outMask; // Stores indexes of bits read from cells
	size_t outputSize;
};
