#pragma once
#include "BaseComponent.h"
#include "PhysicsComponent.h"
#include "Collider.h"
#include "GameObject.h"

#define LinearDragCOEfficient 0.25f;
#define COEfficientOfRestitution 0.5f;
#define CollisionTolerance 2.0f;

class CollisionManager : public BaseComponent
{
public:
	bool m_colliding = true;
	virtual void Update(Time time) override;
	CreateComponentID(CollisionManager);

	void AABB_AABB(GameObject a, GameObject b, CollisionData data);
	void AABB_CIRCLE(GameObject a, GameObject b, CollisionData data);
	void CIRCLE_CIRCLE(GameObject a, GameObject b, CollisionData data);
	void AABB_PLANE(GameObject a, GameObject b, CollisionData data);
	void CIRCLE_PLANE(GameObject a, GameObject b, CollisionData data);
	void AABB_OBB(GameObject a, GameObject b, CollisionData data);
	void OBB_OBB(GameObject a, GameObject b, CollisionData data);
	void AABB_TILEMAP(GameObject a, GameObject b, CollisionData data);

	void RenderComponentMenu(int ID) override;
};

