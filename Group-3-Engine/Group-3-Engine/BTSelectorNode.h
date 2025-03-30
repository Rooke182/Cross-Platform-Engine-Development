#pragma once
#include "BTNode.h"

//Processes all until 1 succeed

class BTSelectorNode : public BTNode
{
public:
	BTSelectorNode(BTNode childNode);
	BTSelectorNode(std::vector<BTNode> childNodes);
	BTState Process() override;
};

