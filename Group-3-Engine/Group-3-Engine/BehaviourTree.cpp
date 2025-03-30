#include "BehaviourTree.h"

BehaviourTree::BehaviourTree()
{ 
	m_rootNode = new BTNode();
}

BehaviourTree::BehaviourTree(std::vector<BTNode> children)
{
	m_rootNode = new BTNode();
	m_rootNode->m_childNodes = children;
}

BTState BehaviourTree::Process()
{
	BTState state = PROCESSING;

	for (auto node : m_rootNode->m_childNodes)
	{
		state = node.Process();
	}

	return state;
}
