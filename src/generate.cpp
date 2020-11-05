/* generate.cpp
 */
#include "generate.hh"
#include "automata.hh"
#include "storage.hh"
#include <stdint.h>
#include <string>

void generate(const std::string &directory, size_t cellSize, size_t outputSize)
{
	try
	{
		Storage files {directory, StorageMode::generate};
		files.setConfig(cellSize, outputSize);
		files.preallocateMap();

		Automata ca {cellSize, outputSize};

		for (uint64_t currentState = 0; currentState < files.getCellCount(); currentState++)
		{
			ca.initState(currentState);

			uint64_t output = ca.getOutput(); // Cannot inline as these must execute in order
			uint64_t nextState = ca.advanceState(); // We also need to save output before advancing

			files.writeMapEntry(currentState, nextState, output);
			files.writeIndex(currentState, output);
		}

	}
	catch (const std::exception &e)
	{
		printf("Unable to generate output!\n%s", e.what());
	}
}
