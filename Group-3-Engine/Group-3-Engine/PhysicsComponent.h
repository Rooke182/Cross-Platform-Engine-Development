#pragma once
#include "BaseComponent.h"
#include "GameObject.h"
#include "Collider.h"
#include <glm/glm.hpp>
class PhysicsComponent : public BaseComponent
{
protected:
	

public: 
	float m_grav = -9.81f;
	float m_dragCoefficient = 0.001f;
	float m_drag = 0.0f;
	float m_mass = 1.0f;
	float m_inverseMass = 0.0f;
	float m_restitution = 0.9f;

	glm::vec3 m_velocity, m_accel, m_netForce = { 0.0f,0.0f,0.0f };

	inline void SetVelocity(glm::vec3 vel) { m_velocity = vel; }
	glm::vec3 GetVelocity() { return m_velocity; }
	inline void SetRestitution(float res) { m_restitution = res; }
	float getRestitution() { return m_restitution; }

	inline void SetAccel(glm::vec3 accel) { m_accel = accel; }

	inline void ApplyImpulse(glm::vec3 pulse) { m_velocity += pulse; }

	inline float getMass() { return m_mass; }
	inline float getInverseMass() { return m_inverseMass; }
	inline void calcualteInverseMass();

	inline void ApplyGravity() { m_netForce.y += 9.81f * m_mass * 100.0f; }

	inline void ApplyDrag();
	inline void NetForce(glm::vec3 force);
	virtual void Update(Time time) override;
	friend class Debugger;
	CreateComponentID(PhysicsComponent);
	SerialiseVariables(6, m_grav, m_drag, m_mass, m_velocity, m_accel, m_netForce);
	friend class DebuggerFunction;
	friend class DebuggerMenus;
	void RenderComponentMenu(int ID) override;

	std::string GetName() override { return "PhysicsComponent"; };
};

