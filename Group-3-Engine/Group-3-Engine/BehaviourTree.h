#pragma once
#include "BTNode.h"
class BehaviourTree
{
public:
	BehaviourTree();
	BehaviourTree(std::vector<BTNode> children);

	BTNode* GetRootNode() { return m_rootNode; }
	BTState Process();

private:
	BTNode* m_rootNode;
};

