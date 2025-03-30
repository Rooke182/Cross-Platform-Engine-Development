#include "PhysicsComponent.h"

inline void PhysicsComponent::calcualteInverseMass() 
{ 
	if (m_mass == 0) return; 
	m_inverseMass = 1.0f / m_mass; 
}

inline void PhysicsComponent::ApplyDrag() {
	//float m_velSquared = glm::dot(m_velocity, m_velocity);
	//if (m_velSquared != 0.0f)
	//{
	//	//m_drag = m_velSquared * 0.01f;
	//	float m_drag = m_velSquared * m_dragCoefficient;
	//	glm::vec3 m_velNorm = glm::normalize(m_velocity);
	//	glm::vec3 m_dragForce = m_velNorm * -m_drag;
	//	m_netForce += m_dragForce;
	//}

	//get the velocity squared to caculate drag
	float velSquared = glm::length(m_velocity);
	velSquared *= velSquared;
	//stops dividing and timesing by 0
	if (velSquared != 0.0f)
	{
		//caculate drag
		m_drag = m_dragCoefficient * velSquared;
		// makes drag face the oposite as velocity to go aginst
		m_netForce += -glm::normalize(m_velocity) * m_drag;
	}
}

inline void PhysicsComponent::NetForce(glm::vec3 force)
{
	m_netForce += force;
}

void PhysicsComponent::Update(Time time)
{
	float delta = time.m_deltaTime * 0.001f;

	m_netForce = { 0.0f, 0.0f, 0.0f };

	calcualteInverseMass();
	//ApplyGravity();
	ApplyDrag();

	m_accel = m_netForce * m_inverseMass;
	m_velocity += m_accel * delta;
	m_parent->GetTransform()->m_position += m_velocity * delta;

	//m_RotationVelocity -= m_RotationVelocity * m_RotationDrag * delta;
}

void PhysicsComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Physics Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		m_accel = DebuggerFunctions::Vec3Input("Acceleration: ", "##object Acceleration", m_accel, true, true, false);
		m_velocity = DebuggerFunctions::Vec3Input("Velocity: ", "##object Velocity", m_velocity, true, true, false);
		m_mass = DebuggerFunctions::FloatInput("Mass: ", "##object mass", m_mass);
		m_dragCoefficient = DebuggerFunctions::FloatInput("Drag Coefficient: ", "##Drag Coefficient: ", m_dragCoefficient);
		m_restitution = DebuggerFunctions::FloatInput("Restotution: ", "##Restotution: ", m_restitution);

		RemoveComponentMenu(ID);
	}
#endif
}
