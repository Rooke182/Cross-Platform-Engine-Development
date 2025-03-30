#pragma once
#include "BaseComponent.h"
#include "NavigationComponent.h"
#include "Vector2.h"
#include "ComponentPtr.h"
#include <stack>
#include "TilemapRenderComponent.h"

class AIAgent;


class SteeringBehaviourComponent : public BaseComponent
{
public:
	friend class Debugger;
	SteeringBehaviourComponent(ComponentPtr<NavigationComponent> navCom);
	SteeringBehaviourComponent() {}

	void Seek(Vector2 endPos);
	void Flee(Vector2 fleePos);
	void WanderHelper();
	void Wander();

	void Init() override;
	void Update(Time time) override;

	Vector2 GetPos() { return m_pos; }
	void SetPos(Vector2 pos) { m_pos = pos; }

protected:
	bool m_wander = false;

	Vector2 m_pos;
	Vector2 m_velocity;
	Vector2 m_acceleration;

	float m_maxSpeed = 0.1;
	const float m_mass = 100.0f;
	const float m_deccelerationFactor = 5.0f;

	Time m_time;

	std::stack<Vector2> m_positionStack;
	std::string GetName() override { return "SteeringBehaviourComponent"; };

	CreateComponentID(SteeringBehaviourComponent);

	ComponentPtr<NavigationComponent> m_navCom;
	ComponentPtr<AIAgent> m_AiAgent;
	ComponentPtr<TilemapRenderComponent> m_tilemapRC;


	enum MovementType
	{
		Wait,
		Move,
		ENUM_COUNT
	};

	float waitTime;
};

