#pragma once
#include <glm/glm.hpp>
#include "BaseComponent.h"
#include "RenderComponent.h"
#include "TilemapData.h"
#include "ComponentPtr.h"
#include <algorithm>
#include <functional>
#include <limits>
class GameObject;

enum ColliderType
{
	NONE,
	AABB,
	CIRCLE,
	PLANE,
	OBB,
	TILEMAP,
};

struct CollisionData
{
	bool m_didCollide = false;
	glm::vec3 m_collisionNorm;
	std::vector<glm::vec2> m_TilePos;
};

struct IntervalResult
{
	float m_min;
	float m_max;
};

class Collider : public BaseComponent
{
protected:
	ColliderType m_type = NONE;

	void Update(Time time) override;
	void RenderComponentMenu(int ID) override;

	static CollisionData AABB_AABB(ComponentPtr<Collider> a, ComponentPtr<Collider> b);
	static CollisionData AABB_CIRCLE(ComponentPtr<Collider> aabb, ComponentPtr<Collider> circle);
	static CollisionData AABB_PLANE(ComponentPtr<Collider> aabb, ComponentPtr<Collider> Plane);
	static CollisionData AABB_OBB(ComponentPtr<Collider> aabb, ComponentPtr<Collider> obb);
	static CollisionData OBB_AABB(ComponentPtr<Collider> aabb, ComponentPtr<Collider> obb);
	static CollisionData CIRCLE_AABB(ComponentPtr<Collider> aabb, ComponentPtr<Collider> circle);
	static CollisionData CIRCLE_CIRCLE(ComponentPtr<Collider> a, ComponentPtr<Collider> b);
	static CollisionData OBB_OBB(ComponentPtr<Collider> a, ComponentPtr<Collider> b);
	static CollisionData CIRCLE_PLANE(ComponentPtr<Collider> circle, ComponentPtr<Collider> plane);

	static CollisionData TILEMAP_AABB(ComponentPtr<Collider> tilemap, ComponentPtr<Collider> aabb);
	static glm::vec2 GetTileWorld(float x, float y, ComponentPtr<Collider> aabb, TilemapData tileMapData);
	static CollisionData TILE_AABB(glm::vec2 position, glm::vec2 size, ComponentPtr<Collider> aabb);

	
	std::array<float, 2> m_origin;
	glm::vec3 m_extents = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_normal = { 1.0f, 0.0f, 0.0f };
public:
	std::function<void(ComponentPtr<Collider>)> m_onCollide;
	std::vector<glm::vec4> m_vertices;
	std::vector<glm::vec3> m_axis;

	//Collider(ColliderType t = NONE) { m_type = t; }
	bool isColliding = false;
	virtual CollisionData CollidesWith(ComponentPtr<Collider> other);

	CreateComponentID(Collider);
	SerialiseVariables(6, m_type, m_vertices, m_axis, m_origin, m_extents, m_normal);

	ColliderType GetType() { return m_type; }
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
	inline glm::vec3 GetPosition();
	inline glm::vec3 GetAngle();
	inline glm::vec3 GetMin();
	inline glm::vec3 GetMax();
	inline glm::vec3 GetExtents();
	inline glm::vec3 GetCentre();
	inline glm::vec3 AABBClosestPoint(glm::vec3 point);
	inline glm::vec3 GetNormals() { return m_normal; }
	inline glm::vec3 GetRotatedNormals();

	inline bool OverlapOnAxis(ComponentPtr<Collider> other, glm::vec3 axis);

	//inline void ComputeAxes(); 
	inline std::vector<glm::vec4> GetCorners();

	inline std::pair<float, float> ProjectPolygon(const std::vector<glm::vec4>& vertices, const glm::vec3& axis);

	inline bool Overlap(float minA, float maxA, float minB, float maxB);


	inline IntervalResult GetInterval(glm::vec3 axis);

	static bool linePoint(float x1, float y1, float x2, float y2, float px, float py);

	void Init() override;
};