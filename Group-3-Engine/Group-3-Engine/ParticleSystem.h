//#pragma once
//#include <iostream>
//#include <vector>
//#include "GameObject.h"
//#include "BaseComponent.h"
//#include "PhysicsComponent.h"
//#include "Collider.h"
//#include "AnimatorComponent.h"
//#include "CollisionManager.h"
//#include "ColliderRenderComponent.h"
//#include "Scene.h"
//#include "PhysicsScene.h"
//
//struct ParticleProps
//{
//    glm::vec2 Position, Velocity, VelocityVariation;
//    glm::vec4 ColorBegin, ColorEnd;
//    float SizeBegin, SizeEnd, SizeVariation;
//    float LifeTime = 1.0f;
//};
//class ParticleSystem : public Scene
//{
//public:
//	ParticleSystem(TransformComponent* tf) : Scene() {}; 
//	//ParticleSystem(TransformComponent* tf, Time time, glm::vec3 pertubation, bool invertGravity);
//    ParticleSystem(Time time);
//	void Reset();
//	void Update(Time deltaTime) override;
//    void OnRender();
//
//    void Emit(const ParticleProps& particleProps);
//private:
//	float resetTime = 500.0f;
//	float timeAlive = 0.0f;
//
//    struct Particle
//    {
//        glm::vec2 Position, Velocity;
//        glm::vec4 ColorBegin, ColorEnd;
//        float Rotation = 0.0f;
//        float SizeBegin, SizeEnd;
//        float LifeTime = 1.0f;
//        float LifeRemaining = 0.0f;
//        bool Active = false;
//    };
//
//    std::vector<Particle> m_ParticlePool; //pre-allocate and reuse as continuous memory
//    uint32_t m_PoolIndex = 999; //index of next particle
//};
//
