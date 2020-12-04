/* statetree.hh
 * Binary tree for storing sets of states
 */
#pragma once
#include <stdint.h>
#include <vector>

// Each set is mapped to our index array
// This way we do not need to store the entirety of each set in each node
struct StateSet
{
	uint64_t index;
	uint64_t length;
};

class StateTree;
class StateNode;

class StateTree
{
public:
	StateTree();
	~StateTree();

	StateNode begin();
	StateNode end();

	// Returns reference to node
	StateSet* &getStates(size_t index);

	inline size_t size()
	{
		return nodes.size();
	}
private:
	std::vector<StateSet*> nodes;
};

// Iterator over nodes in StateTree
class StateNode
{
public:
	StateNode() = delete;
	StateNode(StateTree &tree, size_t treeIndex);
	StateNode(const StateNode &other);
	~StateNode() = default;

	StateSet* &operator*();
	StateSet** operator->();
	StateNode &operator+(size_t index);
	bool operator==(const StateNode &other);
	bool operator!=(const StateNode &other);
	StateNode &operator=(const StateNode &other);

	StateNode parent();
	StateNode left();
	StateNode right();
private:
	StateTree &tree;
	size_t treeIndex;
};
