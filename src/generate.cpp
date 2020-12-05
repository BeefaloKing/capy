/* generate.cpp
 */
#include "generate.hh"
#include "automata.hh"
#include "storage.hh"
#include "utils.hh"
#include <stdint.h>
#include <string>

static const size_t PROGRESS_WIDTH = 50;

void generate(size_t cellSize)
{
	Storage files {cellSize};
	Automata ca {cellSize};

	try
	{
		printf("Generating and sorting index. This will take a long time.\n");
		fflush(stdout);

		double lastProgress = 0;
		printProgressBar(PROGRESS_WIDTH, 0, false); // Print initial progress bar

		do // Initial state has alreadt been set to the root node
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
		while (files.advStateSet());

		printProgressBar(PROGRESS_WIDTH, 1, true); // Print final progress bar at 100%
		printf("Finished sorting index.\n");
		fflush(stdout);

		// Tree requires far more space than index, but tree is stored in ram.
		// This reveals a fundamental design flaw, but is important information.
		// Do not show when compiled in release mode.
		// Professor never has to know :p (unless you are reading this).
		// TLDR; Refactored and nolonger store index on disk.
		// #ifndef NDEBUG
		// files.printTreeSize();
		// #endif
	}
	catch (const std::exception &e)
	{
		printf("Unable to generate index!\n%s", e.what());
	}
}
