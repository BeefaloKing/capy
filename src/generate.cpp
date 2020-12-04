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
		// This is done without reading from the index since it is empty before the first merge
		for (uint64_t currentState = 0; currentState < files.getCellCount(); currentState++)
		{
			ca.initState(currentState);

			files.writeSwap(currentState, ca.getOutput());
		}
		files.mergeSwap();

		while (files.advStateSet())
		{
			size_t depth = files.getDepth();

			uint64_t stateID;
			while (files.getNextState(stateID))
			{
				ca.initState(stateID);

				// We read from the index initial states
				// The depth of the tree represents the size of the walk
				// We need to advance state by the size of the walk to get the nth bit of output
				for (size_t i = 0; i < depth; i++)
				{
					ca.advanceState();
				}

				files.writeSwap(stateID, ca.getOutput());
			}

			files.mergeSwap();
		}
	}
	catch (const std::exception &e)
	{
		printf("Unable to generate output!\n%s", e.what());
	}
}
