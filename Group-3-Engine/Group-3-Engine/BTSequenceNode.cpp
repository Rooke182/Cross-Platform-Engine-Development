#include "BTSequenceNode.h"

BTSequenceNode::BTSequenceNode(BTNode childNode)
{
	m_childNodes.push_back(childNode);
}

BTSequenceNode::BTSequenceNode(std::vector<BTNode> childNodes)
{
	m_childNodes = childNodes;
}

BTState BTSequenceNode::Process()
{
	BTState childState = m_childNodes.at(m_ActiveChild).Process();

	if (childState == SUCCESS)
	{
		m_ActiveChild++;
		if (m_ActiveChild == m_childNodes.size())
		{
			return SUCCESS;
		}
		return PROCESSING;
	}
	else if (childState == FAILURE)
	{
		return FAILURE;
	}

	//returns PROCESSING
	return childState;
}
