#include "ExampleBT.h"
#include "BehaviourTree.h"
#include "BTSelectorNode.h"
#include "BTSequenceNode.h"
#include "AIAgent.h"
#include "SteeringBehaviourComponent.h"


void ExampleBT::BT()
{
	AIAgent* manager;
	std::vector<BTNode> nodeList;

	auto func = [&manager]()
	{
		manager->GetSteeringBehaviourComponent()->Wander();
		if (manager->GetPos().isEqual(Vector2(2.0f, 1.0f)))
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	for (int i = 0; i < 10; i++)
	{
		BTNode node;
		nodeList.push_back(node);
		node.Process(func);
	}

	auto selectNode = BTSelectorNode(nodeList);
	auto sequenceNode = BTSequenceNode(nodeList);

	std::vector<BTNode> childNodes;
	childNodes.push_back(selectNode);
	childNodes.push_back(sequenceNode);

	BehaviourTree* tree = new BehaviourTree(childNodes);

	BTState state = tree->Process();

	if (state == SUCCESS)
	{
		//Do success code here
	}
	else if(state == FAILURE)
	{
		//Do fail code here
	}
}
