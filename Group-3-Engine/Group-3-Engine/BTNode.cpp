#include "BTNode.h"

BTNode::BTNode()
{
	m_ActiveChild = 0;
}

BTState BTNode::Process()
{
	return PROCESSING;
}

BTState BTNode::Process(std::function<bool()> func)
{
	bool ret = func();

	switch (ret)
	{
	case true:
		return SUCCESS;
		break;
	case false:
		return FAILURE;
		break;
	default:
		return PROCESSING;
		break;
	}

	return PROCESSING;
}
