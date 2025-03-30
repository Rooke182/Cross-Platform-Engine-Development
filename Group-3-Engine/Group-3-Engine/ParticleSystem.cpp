//#include "ParticleSystem.h"
//#include "Random.h"
//void ParticleSystem::Update(Time time)
//{   
//	//timeAlive += time.m_deltaTime; //time is a struct with 2 floats
//	//if (timeAlive >= resetTime)
//	//{
//	//	Reset();
//	//}
//
//	for (auto& particle : m_ParticlePool)
//	{
//		if (!particle.Active)
//		{
//			continue;
//		}
//
//		if (particle.LifeRemaining <= 0.0f)
//		{
//			particle.Active = false;
//			continue;
//		}
//
//		particle.LifeRemaining -= time.m_deltaTime;
//		particle.Position += particle.Velocity * time.m_deltaTime;
//		particle.Rotation += 0.01f * time.m_deltaTime; //radians (this stuff could just be passed in from elsewhere)
//	}
//	Scene::Update(time);
//}
//void ParticleSystem::OnRender()
//{
//	for (auto& particle : m_ParticlePool)
//	{
//		if (!particle.Active)
//			continue;
//
//		//fade away particles
//		float life = particle.LifeRemaining / particle.LifeTime;
//		glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
//		color.a = color.a * life;
//		
//		float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
//	}
//}
//void ParticleSystem::Emit(const ParticleProps& particleProps) //creates a particle
//{
//	Particle& particle = m_ParticlePool[m_PoolIndex]; //allocates memory for particle using index
//	particle.Active = true;
//	particle.Position = particleProps.Position;
//	particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();
//
//	//Velocity
//	particle.Velocity = particleProps.Velocity; //base velocity
//	particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f); //add a random amount of velocity onto particle
//	particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);
//
//	//Color
//	particle.ColorBegin = particleProps.ColorBegin;
//	particle.ColorEnd = particleProps.ColorEnd;
//
//	//Life
//	particle.LifeTime = particleProps.LifeTime;
//	particle.LifeRemaining = particleProps.LifeTime;
//	
//	//Size
//	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
//	particle.SizeEnd = particleProps.SizeEnd;
//
//	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size(); //(-- apparently decrements?) sets up pool for next particle
//}
//ParticleSystem::ParticleSystem(Time time)
//{
//	m_ParticlePool.resize(1000); // size of pool
//}
////ParticleSystem::ParticleSystem(TransformComponent* tf, Time time, glm::vec3 pertubation, bool invertGravity) //pertubation is essentially just physics randomness
////{
////	////GetComponentOfType<PhysicsComponent>()->ApplyGravity();
////	//bool isGravityOn = invertGravity;
////	//
////	//auto collisionManager = MakeComponent< CollisionManager>();
////	//auto colid = MakeComponent<Collider>(AABB);
////	//auto colliderRenderer1 = MakeComponent<ColliderRenderComponent>(colid, MakeComponent<TransformComponent>());
////	//auto testobj = AddGameObject(GameObject(std::vector< ComponentPtr<BaseComponent>>{colid, MakeComponent<PhysicsComponent>(), collisionManager, colliderRenderer1}, MakeComponent<TransformComponent>(),
////	//	MakeComponent<SpriteRenderComponent>("assets/textures/indian man.png"), this));
////	//m_gameObjects[testobj].GetTransform()->m_position = { 400.0f, 100.0f, 0.0f }; //change position
////}
//
//void ParticleSystem::Reset()
//{
//	timeAlive = 0.0f;
//	//delete particle here
//}
