/* statetree.cpp
 */
#include "statetree.hh"
#include "automata.hh"
#include <algorithm>
#include <stdexcept>

StateSet::StateSet() :
	index(0),
	length(0),
	depth(0)
{}

StateSet::StateSet(size_t index, size_t length) :
	index(index),
	length(length),
	depth(0)
{}

StateSet::StateSet(size_t index, size_t length, const StateSet &parent) :
	index(index),
	length(length),
	depth(parent.depth + 1)
{}

StateSetQueue::StateSetQueue(size_t index, size_t length, Automata &ca) :
	totalLength(length),
	lostStates(0),
	lostNodes(0),
	lostEntropy(0),
	ca(ca)
{
	// Initialize queue with root node
	toVisit.emplace(index, length);
}

void StateSetQueue::pushChildren(const std::set<uint64_t> &leftSwap,
		const std::set<uint64_t> &rightSwap)
{
	size_t leftSize = leftSwap.size();
	size_t rightSize = rightSwap.size();

	// If all current states output the same bit one of our children will have size 0
	if (leftSize == 0 || rightSize == 0)
	{
		// At least one swap will always have size greater than 0
		// If left size is zero then right size must be greater than 0
		const std::set<uint64_t> &swap = (leftSize > 0) ? leftSwap : rightSwap;

		std::set<uint64_t> future;
		std::set<uint64_t> lookahead {swap};

		// Determine if all entries in swap produce identical output streams
		// Continue advancing each state by one until a cycle has been detected
		// If we reach a cycle, then all states will produce identical output forever
		while (!std::includes(future.begin(), future.end(), lookahead.begin(), lookahead.end()))
		{
			std::set<uint64_t> nextState; // Stores the next state for every entry in swap
			std::set<uint64_t> nextOutput;
			for (const uint64_t &entry : lookahead)
			{
				ca.initState(entry);
				nextState.insert(ca.advanceState());
				nextOutput.insert(ca.getOutput());

				if (nextOutput.size() > 1) // Output differed between states before cycle detected
				{
					goto ADD_CHILDREN;
				}
			}

			future.insert(nextState.begin(), nextState.end());
			lookahead = nextState;
		}

		lostStates += front().length;
		lostEntropy += front().length - 1;
		lostNodes++;
		return;

	}

ADD_CHILDREN:
	// Only create children if they have length > 0
	if (leftSize > 0)
	{
		toVisit.emplace(front().index, leftSize, front());
	}
	if (rightSize > 0)
	{
		toVisit.emplace(front().index + leftSize, rightSize, front());
	}
}
