/* automata.hh
 */
#pragma once
#include <stdint.h>

class Automata
{
public:
	Automata() = delete;
	Automata(size_t cellSize, size_t outputSize);
	~Automata();

	void initState(uint64_t stateID);

	void advanceState();
	uint64_t getOutput();
private:
	uint8_t* cells;
	uint8_t* cellSwap; // Stores "framebuffer" while automata is advancing
	size_t cellSize;
	size_t* outIndex; // Stores indexes of bits read from cells
	size_t outputSize;
};
