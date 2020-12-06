/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include "utils.hh"
#include <stdio.h>
#include <string>
#include <vector>

Storage::Storage(size_t cellSize) :
	cellSize(cellSize),
	stateCount(1ll << cellSize), // Calculates 2^cellSize
	stateIndex(0),
	setQueue(0, stateCount) // Create setQueue with initial root node
{
	// Initialize states with all possible states between 0 and stateCount
	for (size_t i = 0; i < stateCount; i++)
	{
		states.push_back(i);
	}
}

void Storage::writeSwap(uint64_t state, uint64_t output)
{
	switch (output)
	{
		case 0:
			leftSwap.push_back(state);
			break;
		case 1:
			rightSwap.push_back(state);
			break;
		default:
			throwRange("Writing swap entry", 2, output); // Multibit outputs have been deprecated
	}
}

void Storage::mergeSwap()
{
	size_t statesIndex = setQueue.front().index;

	size_t leftSize = leftSwap.size();
	size_t rightSize = rightSwap.size();

	setQueue.pushChildren(leftSize, rightSize);

	for (size_t i = 0; i < leftSize; i++)
	{
		states.at(statesIndex + i) = leftSwap.at(i);
	}
	for (size_t i = 0; i < rightSize; i++)
	{
		states.at(statesIndex + leftSize + i) = rightSwap.at(i);
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

double Storage::getSortProgress()
{
	return (double) setQueue.getLostStates() / (double) setQueue.getTotalLength();
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
