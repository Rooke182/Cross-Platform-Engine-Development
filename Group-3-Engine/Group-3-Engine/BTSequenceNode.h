#pragma once
#include "BTNode.h"


//processes until all succeed or 1 fails
class BTSequenceNode : public BTNode
{
public:
	BTSequenceNode(BTNode childNode);
	BTSequenceNode(std::vector<BTNode> childNodes);

	BTState Process() override;
};