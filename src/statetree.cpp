/* statetree.cpp
 */
#include "statetree.hh"
#include <stdexcept>

StateSet::StateSet() :
	index(0),
	length(0),
	parent(nullptr),
	left(nullptr),
	right(nullptr)
{}

StateSet::StateSet(uint64_t index, uint64_t length, StateSet* parent) :
	index(index),
	length(length),
	parent(parent),
	left(nullptr),
	right(nullptr)
{}

StateSet::~StateSet()
{
	// Recursively deletes all nodes as each child destructs
	delete left;
	delete right;
}

StateSetIterator StateSet::begin()
{
	return StateSetIterator {this};
}

StateSetIterator StateSet::end()
{
	return StateSetIterator {};
}

StateSetIterator::StateSetIterator() :
	current(nullptr),
	depth(0)
{}

StateSetIterator::StateSetIterator(StateSet* current) :
	current(current),
	depth(0)
{}

// Remember right and traverse down left
// Goto last remembered node if left is null
StateSetIterator &StateSetIterator::operator++()
{
	if (!current)
	{
		throw std::out_of_range {"StateSetIterator range check"};
	}

	if (current->right)
	{
		setStack.push_back(current->right);
		depthStack.push_back(depth + 1);
	}

	if (current->left)
	{
		current = current->left;
		depth++;
	}
	else if (!setStack.empty())
	{
		current = setStack.back();
		depth = depthStack.back();
		setStack.pop_back();
		depthStack.pop_back();
	}
	else // Nothing left
	{
		current = nullptr;
	}

	return *this;
}
