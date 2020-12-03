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

		Automata ca {cellSize, outputSize};

		// Initialize swaps
		for (uint64_t currentState = 0; currentState < files.getCellCount(); currentState++)
		{
			ca.initState(currentState);

			files.writeSwap(currentState, ca.getOutput());
		}
		files.mergeSwap();
	}
	catch (const std::exception &e)
	{
		printf("Unable to generate output!\n%s", e.what());
	}
}
