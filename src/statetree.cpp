/* statetree.cpp
 */
#include "statetree.hh"
#include <stdexcept>

StateSet::StateSet() :
	index(0),
	length(0),
	depth(0),
	lastLengthChange(0)
{}

StateSet::StateSet(size_t index, size_t length) :
	index(index),
	length(length),
	depth(0),
	lastLengthChange(0)
{}

StateSet::StateSet(size_t index, size_t length, const StateSet &parent) :
	index(index),
	length(length),
	depth(parent.depth + 1),
	lastLengthChange((parent.length == length) ? parent.lastLengthChange : parent.depth)
{}

StateSetQueue::StateSetQueue(size_t index, size_t length) :
	totalLength(length),
	lostStates(0),
	lostNodes(0)
{
	// Initialize queue with root node
	toVisit.emplace(index, length);
}

void StateSetQueue::pushChildren(size_t leftSize, size_t rightSize)
{
	const StateSet &parent = toVisit.front();

	// If we recieved no information from the last P bits, where P is equal to the number of
	// states in our current set, then no further bits will provide information either.
	if (parent.depth - parent.lastLengthChange >= parent.length)
	{
		lostStates += parent.length;
		lostNodes++;
		return;
	}

	// Only create children if they have length > 0
	if (leftSize > 0)
	{
		toVisit.emplace(parent.index, leftSize, parent);
	}
	if (rightSize > 0)
	{
		toVisit.emplace(parent.index + leftSize, rightSize, parent);
	}
}
