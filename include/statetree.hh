/* statetree.hh
 * Binary tree for storing sets of states
 */
#pragma once
#include "automata.hh"
#include <set>
#include <stdint.h>
#include <queue>

struct StateSet;
class StateSetQueue;
class StateSetIterator;

// Each StateSet stores a subset of all possible states
struct StateSet
{
	StateSet();
	StateSet(size_t index, size_t length);
	StateSet(size_t index, size_t length, const StateSet &parent);
	~StateSet() = default;

	// Each set is mapped to our index array
	// This way we do not need to store the entirety of each set in each node
	size_t index;
	size_t length;
	size_t depth;
};

class StateSetQueue
{
public:
	StateSetQueue() = delete;
	StateSetQueue(size_t index, size_t length, Automata &ca);

	void pushChildren(const std::set<uint64_t> &leftSwap, const std::set<uint64_t> &rightSwap);

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

	size_t getLostStates() const
	{
		return lostStates;
	}

	size_t getLostNodes() const
	{
		return lostNodes;
	}

	size_t getLostEntropy() const
	{
		return lostEntropy;
	}
private:
	size_t totalLength; // Length of the root node
	size_t lostStates; // Count of states ignored due to early out in pushChildren
	size_t lostNodes; // Count of nodes ignored due to early out in pushChildren
	size_t lostEntropy; // Count of entropy lost to inefficiency of internal machine

	std::queue<StateSet> toVisit;
	Automata &ca;
};
