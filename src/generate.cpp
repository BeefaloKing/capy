/* generate.cpp
 */
#include "generate.hh"
#include "automata.hh"
#include "storage.hh"
#include "utils.hh"
#include <math.h>
#include <stdint.h>
#include <string>

static const size_t PROGRESS_WIDTH = 50;

void generate(const std::string &directory, size_t cellSize, size_t outputSize)
{
	try
	{
		Storage files {directory, StorageMode::generate};
		files.setConfig(cellSize, outputSize);

		Automata ca {cellSize, outputSize};

		printf("Generating and sorting index file. This will take a long time.\n");
		fflush(stdout);

		double lastProgress = 0;
		printProgressBar(PROGRESS_WIDTH, 0, false); // Print initial progress bar

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
			size_t depth = files.getSetDepth();

			double progress = files.getSortProgress();
			if (progress >= lastProgress + .01) // Only update progress in increments of .01
			{
				printProgressBar(PROGRESS_WIDTH, progress, false);
				lastProgress = progress;
			}

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

		printProgressBar(PROGRESS_WIDTH, 1, true); // Print final progress bar at 100%
		printf("Finished sorting index file.\n");
		fflush(stdout);
	}
	catch (const std::exception &e)
	{
		printf("Unable to generate output!\n%s", e.what());
	}
}
