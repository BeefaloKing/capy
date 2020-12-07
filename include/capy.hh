/* capy.hh
 */
#pragma once
#include "automata.hh"
#include "storage.hh"
#include <stdint.h>
#include <stdio.h>

class Capy
{
public:
	Capy() = delete;
	Capy(size_t cellSize, size_t outputBit, const std::string &outPath);
	~Capy() = default;

	void mainLoop();
private:
	void makeRecordHeader();
	void makeRecord(size_t depth, size_t nodesSeen, size_t depthSum);

	FILE* outFile; // Stores output record
	Automata ca;
	Storage data;
};
