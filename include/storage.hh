/* storage.hh
 * Used for reading and writing to the stored state/output mapping
 */
#pragma once
#include "statetree.hh"
#include <string>
#include <vector>

enum class StorageMode
{
	generate,
	analyze,
};

class Storage
{
public:
	Storage() = delete;
	Storage(size_t cellSize);
	~Storage() = default;

	size_t getStateCount()
	{
		return stateCount;
	}

	void writeSwap(uint64_t state, uint64_t output);
	// void getIndex(uint64_t output, std::vector<uint64_t> states);

	void mergeSwap();

	// Finds the next non-null and non-empty StateSet
	// Returns false if none exist
	bool advStateSet();

	// Gets next state read from indexFile based on the current StateSet
	// Returns false if there is no next state to read
	bool getNextState(uint64_t &state);

	double getSortProgress(); // Returns progress as value between 0 and 1
	// void printTreeSize();

	// Returns the tree depth of the current StateSet
	size_t getSetDepth() const
	{
		return setQueue.front().depth;
	}

	// Returns the size of the current StateSet
	size_t getSetLength() const
	{
		return setQueue.front().length;
	}

	size_t getLostNodes() const
	{
		return setQueue.getLostNodes();
	}

	size_t getLostStates() const
	{
		return setQueue.getLostStates();
	}
private:
	size_t cellSize;
	size_t stateCount;

	std::vector<uint64_t> states; // Sorted array of all possible automata states
	size_t stateIndex;
	std::vector<uint64_t> leftSwap;
	std::vector<uint64_t> rightSwap;

	StateSetQueue setQueue;
};
