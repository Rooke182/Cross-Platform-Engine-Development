#pragma once
/*
#include "PhysicsScene.h"
#include "PhysicsComponent.h"
#include "ColliderRenderComponent.h"
#include "CollisionManager.h"
#include "RandomNum.h"

struct ParticleProperties
{
    glm::vec2 Position, Velocity, VelocityVariation;
   /* glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 5.0f;
};
class Particles : public PhysicsScene
{
private:
	
	struct Particle
    {
        glm::vec2 m_position, m_velocity, m_velocityVariation;
		
		glm::vec3 m_pertubation; //random movement
		glm::vec3 m_defaultPos = { 0.0f, 0.0f, 0.0f };
   
		float m_timeAlive, m_maxLife;
        bool m_isActive = false;   
		bool m_invertGravity = true;

		// glm::vec4 ColorBegin, ColorEnd;
        //float Rotation = 0.0f;
        // float SizeBegin, SizeEnd;
    };
	 std::vector<Particle> m_ParticlePool; //pre-allocate and reuse as continuous memory
	 uint32_t m_PoolIndex = 999; //index of next particle

public:
	Particles();
	~Particles();

	void Emit(const ParticleProperties& particleProperties);

	void Update(Time time) override;
	//void Reset(Particle& particle);

	glm::vec3 RandomPertubation();

};

*/