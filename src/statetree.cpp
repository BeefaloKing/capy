/* statetree.cpp
 */
#include "statetree.hh"
#include <stdexcept>
#include <stdint.h>
#include <vector>

StateTree::StateTree()
{}

StateTree::~StateTree()
{
	while (!nodes.empty())
	{
		if (nodes.back())
		{
			delete nodes.back();
		}
		nodes.pop_back();
	}
}

StateNode StateTree::begin()
{
	return StateNode {*this, 0};
}

StateNode StateTree::end()
{
	return StateNode {*this, -1ul};
}

StateSet* &StateTree::getStates(size_t index)
{
	if (index == -1ul)
	{
		throw std::out_of_range {"StateTree dereference end() check"};
	}
	if (index >= nodes.size()) // Index of null child node may be out of range
	{
		nodes.resize(index + 1, nullptr); // Resize to index + 1
	}

	return nodes.at(index);
}

StateNode::StateNode(StateTree &tree, size_t treeIndex) :
	tree(tree),
	treeIndex(treeIndex)
{}

StateNode::StateNode(const StateNode &other) :
	tree(other.tree),
	treeIndex(other.treeIndex)
{}

StateSet* &StateNode::operator*()
{
	return tree.getStates(treeIndex);
}

StateSet** StateNode::operator->()
{
	return &tree.getStates(treeIndex);
}

StateNode &StateNode::operator+(size_t index)
{
	treeIndex += index;
	return *this;
}

bool StateNode::operator==(const StateNode &other)
{
	if (&tree != &other.tree)
	{
		return false;
	}

	if (treeIndex == other.treeIndex)
	{
		return true;
	}

	StateSet* thisSet = (treeIndex < tree.size()) ? **this : nullptr;
	StateSet* otherSet = (other.treeIndex < other.tree.size()) ?
		other.tree.getStates(other.treeIndex) : nullptr;

	return thisSet == otherSet;
}

bool StateNode::operator!=(const StateNode &other)
{
	return !(*this == other);
}

StateNode &StateNode::operator=(const StateNode &other)
{
	tree = other.tree;
	treeIndex = other.treeIndex;

	return *this;
}

StateNode StateNode::parent()
{
	if (**this == nullptr)
	{
		throw std::out_of_range {"StateTree range check"};
	}

	size_t parentIndex = (treeIndex - 1) / 2;
	return StateNode {tree, parentIndex};
}

StateNode StateNode::left()
{
	if (**this == nullptr)
	{
		throw std::out_of_range {"StateTree range check"};
	}

	size_t leftIndex = treeIndex * 2 + 1;
	return StateNode {tree, leftIndex};
}

StateNode StateNode::right()
{
	if (**this == nullptr)
	{
		throw std::out_of_range {"StateTree range check"};
	}

	size_t rightIndex = treeIndex * 2 + 2;
	return StateNode {tree, rightIndex};
}
