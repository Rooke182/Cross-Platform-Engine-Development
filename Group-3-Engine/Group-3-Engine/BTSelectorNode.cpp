#include "BTSelectorNode.h"

BTSelectorNode::BTSelectorNode(BTNode childNode)
{
	m_childNodes.push_back(childNode);
}

BTSelectorNode::BTSelectorNode(std::vector<BTNode> childNodes)
{
	m_childNodes = childNodes;
}

BTState BTSelectorNode::Process()
{
	BTState childState = m_childNodes.at(m_ActiveChild).Process();

	if (childState == SUCCESS)
	{
		return SUCCESS;
	}
	else if (childState == FAILURE)
	{
		m_ActiveChild++;

		if (m_ActiveChild == m_childNodes.size())
		{
			return FAILURE;
		}
		return PROCESSING;
	}

	//returns PROCESSING
	return childState;
}
