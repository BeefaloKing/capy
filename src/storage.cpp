/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include "utils.hh"
#include <stdio.h>
#include <string>
#include <vector>

Storage::Storage(size_t cellSize, Automata &ca) :
	cellSize(cellSize),
	stateCount(1ll << cellSize), // Calculates 2^cellSize
	stateIndex(0),
	setQueue(0, stateCount, ca) // Create setQueue with initial root node
{
	// Initialize states with all possible states between 0 and stateCount
	for (size_t i = 0; i < stateCount; i++)
	{
		states.push_back(i);
	}
}

void Storage::writeSwap(uint64_t state, uint64_t output)
{
	std::set<uint64_t>* swap = nullptr;
	switch (output)
	{
		case 0:
			swap = &leftSwap;
			break;
		case 1:
			swap = &rightSwap;
			break;
		default:
			throwRange("Writing swap entry", 2, output); // Multibit outputs have been deprecated
	}

	swap->insert(state);
}

void Storage::mergeSwap()
{
	setQueue.pushChildren(leftSwap, rightSwap);

	auto statesIt = states.begin() + setQueue.front().index;
	for (const uint64_t &entry : leftSwap)
	{
		*statesIt = entry;
		++statesIt;
	}
	for (const uint64_t &entry : rightSwap)
	{
		*statesIt = entry;
		++statesIt;
	}

	leftSwap.clear();
	rightSwap.clear();
}

bool Storage::advStateSet()
{
	setQueue.pop();
	if (!setQueue.empty())
	{
		stateIndex = setQueue.front().index; // Reset stateIndex to index of current StateSet
		return true;
	}

	return false; // No next StateSet
}

bool Storage::getNextState(uint64_t &state)
{
	if (stateIndex < setQueue.front().index + setQueue.front().length)
	{
		state = states.at(stateIndex);

		stateIndex++;
		return true;
	}

	return false;
}

// void Storage::printTreeSize()
// {
// 	auto it = root.begin();
// 	uint64_t nodes = 0;
// 	while (it != root.end())
// 	{
// 		++it;
// 		nodes++;
// 	}
//
// 	printf("Sorting required a tree with %llu nodes.\n", nodes);
// 	printf("This required approximately %s of memory.\n",
// 		getHumanSize(nodes * sizeof(StateSet)).c_str());
// 	fflush(stdout);
// }
