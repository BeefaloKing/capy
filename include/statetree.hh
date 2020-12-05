/* statetree.hh
 * Binary tree for storing sets of states
 */
#pragma once
#include <stdint.h>
#include <vector>

class StateSet;
class StateSetIterator;

// Each StateSet is a node in an intrusive tree
class StateSet
{
public:
	StateSet();
	StateSet(size_t index, size_t length, StateSet* parent = nullptr);
	~StateSet();

	StateSetIterator begin();
	StateSetIterator end();

	// Each set is mapped to our index array
	// This way we do not need to store the entirety of each set in each node
	size_t index;
	size_t length;

	// Intrusive links
	StateSet* parent; // We need the parent link to find ancestors when mergeSwap earlies out
	StateSet* left;
	StateSet* right;
};

// Used to iterate over all nodes in the tree
class StateSetIterator
{
public:
	StateSetIterator();
	StateSetIterator(StateSet* current);
	~StateSetIterator() = default;

	StateSetIterator &operator++();

	operator StateSet*() // Implicit conversion to raw pointer
	{
		return current;
	}

	bool operator==(const StateSetIterator &other)
	{
		return current == other.current;
	}

	bool operator!=(const StateSetIterator &other)
	{
		return current != other.current;
	}

	StateSet &operator*()
	{
		return *current;
	}

	StateSet* operator->()
	{
		return current;
	}

	size_t getDepth()
	{
		return depth;
	}
private:
	StateSet* current;
	size_t depth;

	std::vector<StateSet*> setStack;
	std::vector<size_t> depthStack;
};
