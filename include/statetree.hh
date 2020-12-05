/* statetree.hh
 * Binary tree for storing sets of states
 */
#pragma once
#include <stdint.h>
#include <queue>

class StateSet;
class StateSetQueue;
class StateSetIterator;

// Each StateSet stores a subset of all possible states
class StateSet
{
public:
	StateSet();
	StateSet(size_t index, size_t length);
	StateSet(size_t index, size_t length, const StateSet &parent);
	~StateSet() = default;

	// Each set is mapped to our index array
	// This way we do not need to store the entirety of each set in each node
	size_t index;
	size_t length;
	size_t depth;
	size_t lastLengthChange; // Depth of the most recent ancestor with a different length
};

class StateSetQueue
{
public:
	StateSetQueue() = delete;
	StateSetQueue(size_t index, size_t length);

	void pushChildren(size_t leftSize, size_t rightSize);

	bool empty() const
	{
		return toVisit.empty();
	}

	const StateSet &front() const
	{
		return toVisit.front();
	}

	void pop()
	{
		toVisit.pop();
	}

	size_t getTotalLength() const
	{
		return totalLength;
	}

	size_t getFinishedStates() const
	{
		return finishedStates;
	}

	size_t getFinishedNodes() const
	{
		return finishedNodes;
	}
private:
	size_t totalLength; // Length of the root node
	size_t finishedStates; // Count of states ignored due to early out in pushChildren
	size_t finishedNodes; // Count of nodes ignored due to early out in pushChildren

	std::queue<StateSet> toVisit;
};
