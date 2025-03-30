#include "AIAgent.h"
#include "NavigationComponent.h"
#include "SteeringBehaviourComponent.h"
#include "GameObject.h"
#include "Scene.h"

AIAgent::AIAgent()
{
}

void AIAgent::Init()
{
	//Init NavCom and Steering Component
	m_navigationComponent = m_parent->GetScene()->MakeComponent<NavigationComponent>();
	m_parent->AddComponent(m_navigationComponent);
	m_navigationComponent->Init(200, 200, 1);

	m_steeringComponent = m_parent->GetScene()->MakeComponent<SteeringBehaviourComponent>(m_navigationComponent);
	m_parent->AddComponent(m_steeringComponent);
	m_steeringComponent->Init();

	//Reset wandering Pos and wandering bool
	m_wanderingPos = nullptr;
	m_isWandering = false;
}

void AIAgent::Update(Time time)
{
	Vector2 newPos = m_steeringComponent->GetPos();
	m_parent->GetComponentOfType<TransformComponent>()->m_position = newPos.toGlmVec3();
}

void AIAgent::SetPosition(Vector2 pos) 
{
	m_destination = pos;
}

void AIAgent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "AIAgentComponent##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		//Draw from debugger to get the clicked pos of the tilemap
		m_imGUIdestination[0] = Debugger::m_aISelectedPos.x;
		m_imGUIdestination[1] = Debugger::m_aISelectedPos.y;
		if (ImGui::InputInt2("Destination Input", &m_imGUIdestination[0])) {}
		if (ImGui::Button("Seek"))
		{
			m_steeringComponent->Seek(Vector2(m_imGUIdestination[0], m_imGUIdestination[1]));
		}
		if (ImGui::Button("Flee"))
		{
			m_steeringComponent->Flee(Vector2(m_imGUIdestination[0], m_imGUIdestination[1]));
		}
		if (ImGui::Button("Wander"))
		{
			m_isWandering = !m_isWandering;
			m_steeringComponent->WanderHelper();
		}
		if (ImGui::Checkbox("Wander Box", &m_isWandering)) {}
	}
#endif
}