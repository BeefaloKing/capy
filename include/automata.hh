/* automata.hh
 */
#pragma once
#include <stdint.h>

class Automata
{
public:
	Automata() = delete;
	Automata(size_t cellSize, size_t outputBit, uint8_t rule);
	~Automata() = default;

	void initState(uint64_t stateID);

	uint64_t advanceState(); // Returns the next stateID
	uint64_t getOutput();
private:
	uint64_t cells;
	size_t cellSize;
	uint64_t cellMask;
	size_t outputBit;
	uint8_t rule;
};
