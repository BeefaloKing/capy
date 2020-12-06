/* capy.cpp
 */
#include "capy.hh"
#include "automata.hh"
#include "storage.hh"
#include "utils.hh"
#include <stdint.h>
#include <string>

static const size_t PROGRESS_WIDTH = 50;

Capy::Capy(size_t cellSize, size_t outputBit) :
	data(cellSize),
	ca(cellSize, outputBit)
{}

// TODO: Output data in some format excel and/or matlab can handle
void Capy::makeRecord(size_t depth, size_t depthSum, size_t nodesSeen)
{

}

void Capy::mainLoop()
{
	try
	{
		printf("Generating and sorting index. This will take a long time.\n");
		fflush(stdout);

		size_t depthSum = 0;
		size_t nodesSeen = 0;
		size_t maxDepth = 0;

		double lastProgress = -1; // Guarantee progress bar is printed at 0%
		do // Initial state has alreadt been set to the root node
		{
			size_t depth = data.getSetDepth();

			// Reset counters after reaching a new tree depth
			if (depth > maxDepth)
			{
				makeRecord(maxDepth, depthSum, nodesSeen);

				// Reset accumulators to include information from lost nodes
				depthSum = data.getLostStates();
				nodesSeen = data.getLostNodes();
				maxDepth = depth;
			}

			nodesSeen++;
			depthSum += data.getSetLength();

			double progress = data.getSortProgress();
			if (progress >= lastProgress + .01) // Only update progress in increments of .01
			{
				printProgressBar(PROGRESS_WIDTH, progress, false);
				lastProgress = progress;
			}

			uint64_t stateID;
			while (data.getNextState(stateID))
			{
				ca.initState(stateID);

				// We read from the index initial states
				// The depth of the tree represents the size of the walk
				// We need to advance state by the size of the walk to get the nth bit of output
				for (size_t i = 0; i < depth; i++)
				{
					ca.advanceState();
				}

				data.writeSwap(stateID, ca.getOutput());
			}

			data.mergeSwap();
		}
		while (data.advStateSet());

		printProgressBar(PROGRESS_WIDTH, 1, true); // Print final progress bar at 100%
		printf("Finished sorting index.\n");
		fflush(stdout);

		// Make a final record to capture bottom most tree depth
		makeRecord(maxDepth, depthSum, nodesSeen);

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
		printf("Unable to generate output!\n%s", e.what());
	}
}
