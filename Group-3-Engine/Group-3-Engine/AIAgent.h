#pragma once
#include "BaseComponent.h"
#include "Vector2.h"
#include "SteeringBehaviourComponent.h"
#include "ComponentPtr.h"
#include "Application.h"
#include <stack>

class NavigationComponent;

class AIAgent : public BaseComponent
{
public:

	friend class SteeringBehaviourComponent;
	//TODO: Add basic functionality
	AIAgent();
	void Init() override;
	void Update(Time time) override;
	void Render() override {};
	void Cleanup() override {};
	Vector2 GetPos() { return m_pos; }

	ComponentPtr<SteeringBehaviourComponent> GetSteeringBehaviourComponent() { return m_steeringComponent; }
	ComponentPtr<NavigationComponent> GetNavigationComponent() { return m_navigationComponent; }

	void SetPosition(Vector2 pos);
	void RenderComponentMenu(int ID) override;

protected:
	ComponentPtr<NavigationComponent> m_navigationComponent;
	ComponentPtr<SteeringBehaviourComponent> m_steeringComponent;

	friend class Debugger;
	friend class Collider;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;

	bool m_isWandering = false;
	Vector2 m_pos;
	Vector2 m_destination;
	int m_imGUIdestination[2];
	Vector2* m_wanderingPos;
	const float m_stoppingdistance = 2.0f;
	float m_maxSpeed = 0.1f;

	CreateComponentID(AIAgent);
};
