/* capy.hh
 */
#pragma once
#include "automata.hh"
#include "storage.hh"
#include <stdint.h>

class Capy
{
public:
	Capy() = delete;
	Capy(size_t cellSize, size_t outputBit);
	~Capy() = default;

	void mainLoop();
private:
	void makeRecord(size_t depth, size_t depthSum, size_t nodesSeen);

	Storage data;
	Automata ca;
};
