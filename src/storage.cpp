/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include "utils.hh"
#include <stdio.h>
#include <string>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

Storage::Storage(size_t cellSize) :
	cellSize(cellSize),
	stateCount(1ll << cellSize), // Calculates 2^cellSize
	finishedSorting(0),
	stateIndex(0),
	root(StateSet {0, stateCount}), // Root node will always point to all states
	currentSet(root.begin()) // Initialize currentSet to root
{
	// Initialize states with all possible states between 0 and stateCount
	for (size_t i = 0; i < stateCount; i++)
	{
		states.push_back(i);
	}
}

void Storage::writeSwap(uint64_t state, uint64_t output)
{
	// Also relies on little endian architecture
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
	size_t statesIndex = currentSet->index;

	size_t leftSize = leftSwap.size();
	size_t rightSize = rightSwap.size();

	// If one child contains no states (and the other contains the same states as the parent),
	// then this particular bit of output does not contain any information.
	// If we recieved no information from the last P bits, where P is equal to the number of
	// states in our current set, then no further bits will provide information either.
	// Effectively, we are unable to determine the state of the machine,
	// but can determine the entire rest of the bitstream.
	// Early out, as there is no need to continue generating nodes down this path.
	bool earlyOut = false;
	if (leftSize == 0 || rightSize == 0)
	{
		earlyOut = true;

		StateSet* ancestor = currentSet;
		for (size_t i = 0; i < currentSet->length; i++)
		{
			ancestor = ancestor->parent;
			if (ancestor && ancestor->length == currentSet->length)
			{
				continue;
			}
			else
			{
				// We recieved information less than P bits ago
				// It is not yet safe to early out
				earlyOut = false;
				break;
			}
		}
	}

	if (earlyOut) // We have finised sorting as many elements are in currentSet
	{
		finishedSorting += currentSet->length;
	}
	else
	{
		currentSet->left = (leftSize > 0) ?
			new StateSet {statesIndex, leftSize, currentSet} : nullptr;
		currentSet->right = (rightSize > 0) ?
			new StateSet {statesIndex + leftSize, rightSize, currentSet} : nullptr;
	}

	for (size_t i = 0; i < leftSwap.size(); i++)
	{
		states.at(statesIndex + i) = leftSwap.at(i);
	}
	for (size_t i = 0; i < rightSwap.size(); i++)
	{
		states.at(statesIndex + leftSize + i) = rightSwap.at(i);
	}

	leftSwap.clear();
	rightSwap.clear();
}

bool Storage::advStateSet()
{
	while (++currentSet != root.end())
	{
		if (currentSet->length != 0) // Next valid StateSet found
		{
			stateIndex = currentSet->index; // Set stateIndex used by getNextState()
			return true;
		}
	}

	return false; // No next valid StateSet
}

size_t Storage::getSetDepth()
{
	return currentSet.getDepth();
}

bool Storage::getNextState(uint64_t &state)
{
	if (stateIndex < currentSet->index + currentSet->length)
	{
		state = states.at(stateIndex);

		stateIndex++;
		return true;
	}

	return false;
}

double Storage::getSortProgress()
{
	return (double) finishedSorting / (double) root.length;
}

void Storage::printTreeSize()
{
	auto it = root.begin();
	uint64_t nodes = 0;
	while (it != root.end())
	{
		++it;
		nodes++;
	}

	printf("Sorting required a tree with %llu nodes.\n", nodes);
	printf("This required approximately %s of memory.\n",
		getHumanSize(nodes * sizeof(StateSet)).c_str());
	fflush(stdout);
}
