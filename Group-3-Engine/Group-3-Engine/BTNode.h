#pragma once
#include <vector>
#include "Structs.h"
#include <functional>

class BTNode
{
public:
	BTNode();

	//pass in lambda?
	virtual BTState Process();

	BTState Process(std::function<bool()> func);

	std::vector<BTNode> m_childNodes;
	//TODO: BLACKBOARD REF
	int m_ActiveChild;
	BTNode* m_parent;
};
