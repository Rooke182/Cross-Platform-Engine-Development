#include "Particles.h"
/*
Particles::Particles()
{
	/*auto collisionManager = MakeComponent< CollisionManager>();
	auto col1 = MakeComponent<Collider>(AABB);
	auto colliderRenderer1 = MakeComponent<ColliderRenderComponent>(col1, MakeComponent<TransformComponent>());
	auto object = AddGameObject(GameObject(std::vector< ComponentPtr<BaseComponent>>{col1, MakeComponent<PhysicsComponent>(), collisionManager, colliderRenderer1}, MakeComponent<TransformComponent>(),
		MakeComponent<SpriteRenderComponent>("assets/textures/burger.jpg"), this));
}
Particles::~Particles()
{
}
void Particles::Emit(const ParticleProperties& particleProperties) //creates a particle
{
	Particle& particle = m_ParticlePool[m_PoolIndex]; //allocates memory for particle using index
	particle.m_isActive = true;
	particle.m_position = particle.m_position;
	//particle.m_rotation = Random::Float() * 2.0f * glm::pi<float>();

	//Velocity
	particle.m_velocity = particle.m_velocity; //base velocity
	particle.m_velocity.x += particle.m_velocityVariation.x * (RandomNum::Float() - 0.5f); //add a random amount of velocity onto existing baseVelocity
	particle.m_velocity.y += particle.m_velocityVariation.y * (RandomNum::Float() - 0.5f);

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size(); //(-- apparently decrements?) sets up pool for next particle

	//Life
	particle.m_maxLife = particleProperties.LifeTime;
	particle.m_timeAlive = 0.0f;
	
	
}
void Particles::Update(Time time)
{
	//checks if particle is still active
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.m_isActive)
		{
			continue;
		}

		if (particle.m_maxLife <= particle.m_timeAlive)
		{
			particle.m_isActive = false;
			continue;
		}

		particle.m_timeAlive += time.m_deltaTime;
		particle.m_position += particle.m_velocity * time.m_deltaTime;
		//particle.Rotation += 0.01f * time.m_deltaTime; //radians (this stuff could just be passed in from elsewhere)
		std::cout << "Time alive: " << particle.m_timeAlive << std::endl;
		std::cout << "pertubation: " << particle.m_pertubation.x << " " << particle.m_pertubation.y << std::endl;

		PhysicsScene::Update(time);
	}
}
//m_ParticlePool[m_PoolIndex].m_timeAlive += time.m_deltaTime; //time is a struct with 2 floats
//if (particle.m_timeAlive >= particle.m_resetTime)
//{
///*	Reset();*/ //dont know if i need this since every particle has their own timer?
//}	
//void Particles::Reset(Particle& particle)
//{
//	m_timeAlive = 0.0f;
//
//	//need to change this to set the particle to be reset (I assume thats gameObj 2 but idk)
//	m_gameObjects[2].GetTransform()->m_position = particle.m_defaultPos; //change position
//}
/*
glm::vec3 Particles::RandomPertubation()
{
	//should just get a random float between 0 and 10 for x and y

	glm::vec3 rand = glm::vec3(RandomNum::Float() * 10, RandomNum::Float() * 10, 0);
	return rand;
}*/

