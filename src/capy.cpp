/* capy.cpp
 */
#include "capy.hh"
#include "automata.hh"
#include "error.hh"
#include "storage.hh"
#include "utils.hh"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static const size_t PROGRESS_WIDTH = 50;

Capy::Capy(size_t cellSize, size_t outputBit, uint8_t rule, const std::string &outPath) :
	outFile(fopen(outPath.c_str(), "w")),
	ca(cellSize, outputBit, rule),
	data(cellSize, ca)
{
	if (outFile == nullptr)
	{
		throwFileAccess(outPath);
	}

	printf("Storing analysis in %s.\n", outPath.c_str());
	fflush(stdout);

	makeRecordHeader();
}

void Capy::makeRecordHeader()
{
	fprintf(outFile, "bitstream_size,distinct_bitstreams,distinct_states\n");
}

void Capy::makeRecord(size_t depth, size_t nodesSeen, size_t depthSum)
{
	fprintf(outFile, "%zu,%zu,%zu\n", depth, nodesSeen, depthSum);
}

void Capy::mainLoop()
{
	printf("Reversing CAPRNG. This will take a long time.\n");
	fflush(stdout);

	size_t depthSum = 0;
	size_t nodesSeen = 0;
	size_t maxDepth = 0;
	size_t maxSum = 0; // Used for progress

	// Print initial progress bar
	printProgressBar(PROGRESS_WIDTH, 0, false);

	do // Initial state has alreadt been set to the root node
	{
		size_t depth = data.getSetDepth();

		// Reset counters after reaching a new tree depth
		if (depth > maxDepth)
		{
			// Subtract lost entropy from depthSum before making record
			depthSum -= data.getLostEntropy();
			makeRecord(maxDepth, nodesSeen, depthSum);

			maxSum = std::max(maxSum, depthSum);
			double probProg = log2(nodesSeen) / log2(depthSum); // This just felt right
			double stateProg = (maxSum - depthSum) / (double) maxSum;
			printProgressBar(PROGRESS_WIDTH, std::max(probProg, stateProg), false);

			// Reset accumulators to include information from lost nodes
			// depthSum = data.getLostStates();
			depthSum = data.getLostStates();
			nodesSeen = data.getLostNodes();
			maxDepth = depth;
		}

		nodesSeen++;
		depthSum += data.getSetLength();

		uint64_t stateID;
		while (data.getNextState(stateID))
		{
			ca.initState(stateID);

			uint64_t output = ca.getOutput();
			uint64_t nextState = ca.advanceState();

			data.writeSwap(nextState, output);
		}

		data.mergeSwap();
	}
	while (data.advStateSet());

	printProgressBar(PROGRESS_WIDTH, 1, true); // Print final progress bar at 100%
	printf("Finished reversing all output streams.\n");
	fflush(stdout);

	// Make a final record to capture bottom most tree depth
	depthSum -= data.getLostEntropy();
	makeRecord(maxDepth, nodesSeen, depthSum);

	// Tree requires far more space than index, but tree is stored in ram.
	// This reveals a fundamental design flaw, but is important information.
	// Do not show when compiled in release mode.
	// Professor never has to know :p (unless you are reading this).
	// TLDR; Refactored and nolonger store index on disk.
	// #ifndef NDEBUG
	// files.printTreeSize();
	// #endif
}
