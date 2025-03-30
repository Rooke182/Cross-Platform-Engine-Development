#include "CollisionManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "SpriteRenderComponent.h"
#include "TilemapRenderComponent.h"

// i had to clean this cause this was a giant mess man   
void CollisionManager::Update(Time time)
{
    m_parent->GetScene()->IterateGameObjects([&](GameObject& object1)
        {
            auto collider1 = object1.GetComponentOfType<Collider>();
            if (collider1 == nullptr)return;

            object1.GetScene()->IterateGameObjects([&](GameObject& object2)
            {
                // Avoid self-collision and redundant checks
                if (&object1 >= &object2) return;

                auto collider2 = object2.GetComponentOfType<Collider>();
                if (collider2 == nullptr) return;

                // Perform collision detection
                auto data = collider1->CollidesWith(collider2);
                if (!data.m_didCollide) return;

                // Set collision flag
                collider1->isColliding = true;
                collider2->isColliding = true;

                    if (!m_colliding) return;
                    //Debugger::AddLog("GO1: %s, GO2: %s", &object1.GetComponentOfType<SpriteRenderComponent>()->m_texturePath, &object2.GetComponentOfType<SpriteRenderComponent>()->m_texturePath);

                if (collider1->GetType() == AABB && collider2->GetType() == AABB) AABB_AABB(object1, object2, data);

                if (collider1->GetType() == CIRCLE && collider2->GetType() == CIRCLE) CIRCLE_CIRCLE(object1, object2, data);

                    if (collider1->GetType() == OBB && collider2->GetType() == OBB) OBB_OBB(object1, object2, data);

                    if (collider1->GetType() == AABB && collider2->GetType() == CIRCLE) AABB_CIRCLE(object1, object2, data);
                    if (collider2->GetType() == AABB && collider1->GetType() == CIRCLE) AABB_CIRCLE(object2, object1, data);

                if (collider1->GetType() == AABB && collider2->GetType() == PLANE) AABB_PLANE(object1, object2, data);
                if (collider2->GetType() == AABB && collider1->GetType() == PLANE) AABB_PLANE(object2, object1, data);

                if (collider1->GetType() == CIRCLE && collider2->GetType() == PLANE) CIRCLE_PLANE(object1, object2, data);
                if (collider2->GetType() == CIRCLE && collider1->GetType() == PLANE) CIRCLE_PLANE(object2, object1, data);

                if (collider1->GetType() == OBB && collider2->GetType() == OBB) OBB_OBB(object1, object2, data);
                
                if (collider1->GetType() == AABB && collider2->GetType() == OBB) AABB_OBB(object1, object2, data);
                if (collider2->GetType() == AABB && collider1->GetType() == OBB) AABB_OBB(object2, object1, data);

                if (collider1->GetType() == AABB && collider2->GetType() == TILEMAP) AABB_TILEMAP(object1, object2, data);
                if (collider2->GetType() == AABB && collider1->GetType() == TILEMAP) AABB_TILEMAP(object2, object1, data);

                if(collider1->m_onCollide != nullptr)
                collider1->m_onCollide(collider2);
                if (collider2->m_onCollide != nullptr)
                collider2->m_onCollide(collider1);
            });
    });
}

void CollisionManager::AABB_AABB(GameObject a, GameObject b, CollisionData data)
{
    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    glm::vec2 aPos = aTrans->m_position;
    glm::vec2 aSize = { aTrans->m_width * aTrans->m_scale.x, aTrans->m_height * aTrans->m_scale.y };
    glm::vec2 bPos = bTrans->m_position;
    glm::vec2 bSize = { bTrans->m_width * bTrans->m_scale.x, bTrans->m_height * bTrans->m_scale.y };

    glm::vec2 a_min = aPos;
    glm::vec2 a_max = aPos + aSize;
    glm::vec2 b_min = bPos;
    glm::vec2 b_max = bPos + bSize;

    glm::vec3 collisionNormal = glm::normalize(data.m_collisionNorm);

    float overlapX = std::min(a_max.x - b_min.x, b_max.x - a_min.x);
    float overlapY = std::min(a_max.y - b_min.y, b_max.y - a_min.y);

    if (overlapX < overlapY)
    {
        if (collisionNormal.x < 0)
        {
            if (aPhys->getMass() != 0)
            aTrans->m_position.x -= overlapX * 0.5f;
            if (bPhys->getMass() != 0)
            bTrans->m_position.x += overlapX * 0.5f;
        }
        else
        {
            if (aPhys->getMass() != 0)
            aTrans->m_position.x += overlapX * 0.5f;
            if (bPhys->getMass() != 0)
            bTrans->m_position.x -= overlapX * 0.5f;
        }
    }
    else
    {
        if (collisionNormal.y < 0)
        {
            if (aPhys->getMass() != 0)
            aTrans->m_position.y -= overlapY * 0.5f;
            if (bPhys->getMass() != 0)
            bTrans->m_position.y += overlapY * 0.5f;
        }
        else
        {
            if (aPhys->getMass() != 0)
            aTrans->m_position.y += overlapY * 0.5f;
            if (bPhys->getMass() != 0)
            bTrans->m_position.y -= overlapY * 0.5f;
        }
    }

    glm::vec2 relativeVelocity = bPhys->GetVelocity() - aPhys->GetVelocity();
    float velocityAlongNormal = glm::dot(relativeVelocity, glm::vec2(collisionNormal));

    if (velocityAlongNormal == 0)
    {
        return;
    }

    float e = std::min(aPhys->getRestitution(), bPhys->getRestitution());

    float j = -(1 + e) * velocityAlongNormal;
    j /= 1 / aPhys->getMass() + 1 / bPhys->getMass();

    glm::vec2 impulse = j * glm::vec2(collisionNormal);
    impulse = glm::abs(impulse);
    if (aPhys->getMass() != 0)
        aPhys->SetVelocity(aPhys->GetVelocity() - glm::vec3((impulse * aPhys->getInverseMass()), 0));

    if (bPhys->getMass() != 0)
        bPhys->SetVelocity(bPhys->GetVelocity() + glm::vec3((impulse * bPhys->getInverseMass()), 0));
}

void CollisionManager::AABB_CIRCLE(GameObject a, GameObject b, CollisionData data)
{
    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    glm::vec3 aPos = aTrans->m_position;
    glm::vec3 aSize = { aTrans->m_width * aTrans->m_scale.x, aTrans->m_height * aTrans->m_scale.y,0 };

    glm::vec3 aMin = aPos;
    glm::vec3 aMax = aPos + aSize;

    float bRadius = (bTrans->m_width * bTrans->m_scale.x) * 0.5;
    glm::vec3 bPos = { bTrans->m_position.x + bRadius, bTrans->m_position.y + bRadius, 0 };

    glm::vec3 closestPoint;
    closestPoint.x = std::max(aMin.x, std::min(bPos.x, aMax.x));
    closestPoint.y = std::max(aMin.y, std::min(bPos.y, aMax.y));
    closestPoint.z = 0;

    glm::vec3 distance = closestPoint - bPos;
    float distanceSquared = glm::dot(distance, distance);
    float radiusSquared = bRadius * bRadius;

    float distanceMagnitude = glm::sqrt(distanceSquared);
    glm::vec3 collisionNormal;

    if (distanceMagnitude == 0)
    {
        collisionNormal = glm::vec3(1, 0, 0);
    }
    else {
        collisionNormal = distance / distanceMagnitude;
    }

    float penetrationDepth = bRadius - distanceMagnitude;

    float totalInverseMass = aPhys->getInverseMass() + bPhys->getInverseMass();

    if (totalInverseMass == 0) {
        return;
    }

    glm::vec3 correction = collisionNormal * (penetrationDepth / totalInverseMass);
    if (bPhys->getMass() != 0)
        bTrans->m_position -= correction * bPhys->getInverseMass();
    if (aPhys->getMass() != 0)
        aTrans->m_position += correction * aPhys->getInverseMass();

    glm::vec3 relativeVelocity = bPhys->m_velocity - aPhys->m_velocity;
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    float e = std::min(aPhys->getRestitution(), bPhys->getRestitution());
    float j = -(1 + e) * velocityAlongNormal;

    j /= totalInverseMass;

    glm::vec3 impulse = j * collisionNormal;
    if (aPhys->getMass() != 0)
        aPhys->m_velocity -= impulse * aPhys->getInverseMass();
    if (bPhys->getMass() != 0)
        bPhys->m_velocity += impulse * bPhys->getInverseMass();
}

void CollisionManager::CIRCLE_CIRCLE(GameObject a, GameObject b, CollisionData data)
{
    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    float aRad = (aTrans->m_width * aTrans->m_scale.x) * 0.5;
    float bRad = (bTrans->m_width * bTrans->m_scale.x) * 0.5;

    glm::vec2 aPos = glm::vec2(aTrans->m_position) + glm::vec2(aRad, aRad);
    glm::vec2 bPos = glm::vec2(bTrans->m_position) + glm::vec2(bRad, bRad);

    glm::vec2 delta = bPos - aPos;
    float distance = glm::length(delta);

    glm::vec3 normal = glm::vec3(glm::normalize(delta),0);

    float overlap = (aRad + bRad) - distance;
    glm::vec3 mtd = normal * overlap;

    if (bPhys->getMass() == 0)
        aTrans->m_position -= mtd;
    if (aPhys->getMass() == 0)
        bTrans->m_position += mtd;


    glm::vec3 relativeVelocity = bPhys->m_velocity - aPhys->m_velocity;
    float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

    if (relativeVelocityAlongNormal > 0) {
        return;
    }

    float e = std::min(aPhys->m_restitution, bPhys->m_restitution); 
    float j = -(1 + e) * relativeVelocityAlongNormal / (aPhys->getInverseMass() + bPhys->getInverseMass());

    glm::vec3 impulse = j * normal;
    if (bPhys->getMass() == 0)
        aPhys->m_velocity -= impulse * aPhys->getInverseMass();
    if (bPhys->getMass() == 0)
        bPhys->m_velocity += impulse * bPhys->getInverseMass();
}

//a is aabb b is plane
void CollisionManager::AABB_PLANE(GameObject a, GameObject b, CollisionData data)
{
    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aCol = a.GetComponentOfType<Collider>();
    auto bCol = b.GetComponentOfType<Collider>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    // Get the plane normal from the collider component of the plane
    glm::vec3 planeNormal = bCol->GetRotatedNormals();

    // Calculate the plane distance from the origin
    float planeDistance = -glm::dot(planeNormal, bTrans->m_position);

    // Get the min and max points of the AABB
    glm::vec3 aMin = aCol->GetMin();
    glm::vec3 aMax = aCol->GetMax();

    // Define the vertices of the AABB
    glm::vec3 vertices[8] = {
        {aMin.x, aMin.y, aMin.z},
        {aMax.x, aMin.y, aMin.z},
        {aMin.x, aMax.y, aMin.z},
        {aMax.x, aMax.y, aMin.z},
        {aMin.x, aMin.y, aMax.z},
        {aMax.x, aMin.y, aMax.z},
        {aMin.x, aMax.y, aMax.z},
        {aMax.x, aMax.y, aMax.z}
    };

    bool hasPositive = false;
    bool hasNegative = false;

    for (const auto& vertex : vertices) {
        float distance = glm::dot(planeNormal, vertex) + planeDistance;
        if (distance > 0)
            hasPositive = true;
        else if (distance < 0)
            hasNegative = true;

        if (hasPositive && hasNegative)
            break;
    }

    float minPenetration = FLT_MAX;
    for (const auto& vertex : vertices) {
        float distance = glm::dot(planeNormal, vertex) + planeDistance;
        minPenetration = std::min(minPenetration, abs(distance));
    }

    glm::vec3 resolutionVector = planeNormal * minPenetration;

    float centerDistance = glm::dot(planeNormal, aTrans->m_position+glm::vec3(aTrans->m_width/2, aTrans->m_height/2,0)) + planeDistance;

    if (centerDistance < 0) 
    {
        resolutionVector = -resolutionVector;
        planeNormal = -planeNormal;
    }

    aTrans->m_position += resolutionVector;

    glm::vec3 velocity = aPhys->GetVelocity();
    float velocityDot = glm::dot(velocity, planeNormal);

    if (velocityDot < 0) {
        // Reflect the velocity off the plane
        glm::vec3 reflection = velocity - 2 * velocityDot * planeNormal;
        aPhys->SetVelocity(reflection);
    }
}

//a is circle b is plane
void CollisionManager::CIRCLE_PLANE(GameObject a, GameObject b, CollisionData data) 
{
    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aCol = a.GetComponentOfType<Collider>();
    auto bCol = b.GetComponentOfType<Collider>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    glm::vec3 planeNormal = bCol->GetRotatedNormals();

    float planeDistance = -glm::dot(planeNormal, bTrans->m_position);

    float sphereRadius = aTrans->m_width * aTrans->m_scale.x * 0.5;
    glm::vec3 sphereCenter = aTrans->m_position + glm::vec3(sphereRadius, sphereRadius,0);

    float centerDistance = glm::dot(planeNormal, sphereCenter) + planeDistance;

    bool isIntersecting = std::abs(centerDistance) <= sphereRadius;

    if (isIntersecting) 
    {
        float penetrationDepth = sphereRadius - std::abs(centerDistance);

        glm::vec3 resolutionVector = planeNormal * penetrationDepth;

        if (centerDistance < 0) 
        {
            resolutionVector = -resolutionVector;
            planeNormal = -planeNormal;
        }
        aTrans->m_position += resolutionVector;

        glm::vec3 velocity = aPhys->GetVelocity();
        float velocityDot = glm::dot(velocity, planeNormal);

        if (velocityDot < 0) 
        {
            glm::vec3 reflection = velocity - 2 * velocityDot * planeNormal;
            aPhys->SetVelocity(reflection);
        }
    }
}
void CollisionManager::AABB_OBB(GameObject a, GameObject b, CollisionData data)
{

    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aCol = a.GetComponentOfType<Collider>();
    auto bCol = b.GetComponentOfType<Collider>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    auto aCor = aCol->m_vertices;
    auto bCor = bCol->m_vertices;

    auto aAxis = aCol->m_axis;
    auto bAxis = bCol->m_axis;

    //merge axes
    std::vector<glm::vec3> axis;
    axis.insert(axis.end(), aAxis.begin(), aAxis.end());
    axis.insert(axis.end(), bAxis.begin(), bAxis.end());

    // Initialize variables to find the smallest penetration depth and the collision normal
    float smallestPenetration = std::numeric_limits<float>::max();
    glm::vec3 collisionNormal;

    // Check each axis for the smallest penetration depth
    for (const auto& axis : axis) {
        auto normalizedAxis = glm::normalize(axis);
        auto [minProj1, maxProj1] = aCol->ProjectPolygon(aCor, normalizedAxis);
        auto [minProj2, maxProj2] = bCol->ProjectPolygon(bCor, normalizedAxis);

        if (!aCol->Overlap(minProj1, maxProj1, minProj2, maxProj2)) {
            continue;
        }

        // Calculate the penetration depth on this axis
        float penetration = std::min(maxProj1, maxProj2) - std::max(minProj1, minProj2);
        if (penetration < smallestPenetration) {
            smallestPenetration = penetration;
            collisionNormal = normalizedAxis;
        }
    }

    // Normalize the collision normal
    collisionNormal = glm::normalize(collisionNormal);

    // Resolve the collision by separating the objects
    glm::vec3 resolutionVector = collisionNormal * smallestPenetration;

    // Displace objects to resolve the collision
    //float totalMass = aPhys->m_mass + bPhys->m_mass;
    glm::vec3 displacementA = resolutionVector;// *(bPhys->m_mass / totalMass);
    glm::vec3 displacementB = resolutionVector;// *(aPhys->m_mass / totalMass);

    aTrans->m_position -= displacementA;
    bTrans->m_position += displacementB;

    // Apply impulse to simulate the collision response (bounce)
    glm::vec3 relativeVelocity = bPhys->m_velocity - aPhys->m_velocity;
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    if (velocityAlongNormal > 0) return;

    // Calculate restitution (bounciness)
    float e = std::min(aPhys->m_restitution, bPhys->m_restitution);

    // Calculate impulse scalar
    float j = -(1 + e) * velocityAlongNormal;
    j /= (aPhys->m_inverseMass + bPhys->m_inverseMass);

    // Apply impulse
    glm::vec3 impulse = j * collisionNormal;
    aPhys->m_velocity -= impulse * aPhys->m_inverseMass;
    bPhys->m_velocity += impulse * bPhys->m_inverseMass;
}

void CollisionManager::OBB_OBB(GameObject a, GameObject b, CollisionData data)
{
    auto aPhys = a.GetComponentOfType<PhysicsComponent>();
    auto bPhys = b.GetComponentOfType<PhysicsComponent>();

    auto aCol = a.GetComponentOfType<Collider>();
    auto bCol = b.GetComponentOfType<Collider>();

    auto aTrans = a.GetComponentOfType<TransformComponent>();
    auto bTrans = b.GetComponentOfType<TransformComponent>();

    // Corners positions of the rects
    auto aCor = aCol->m_vertices;
    auto bCor = bCol->m_vertices;

    auto aAxis = aCol->m_axis;
    auto bAxis = bCol->m_axis;

    std::vector<glm::vec3> axis;
    axis.insert(axis.end(), aAxis.begin(), aAxis.end());
    axis.insert(axis.end(), bAxis.begin(), bAxis.end());

    // Variables to store the minimum translation vector (MTV)
    float minOverlap = FLT_MAX;
    glm::vec3 mtvAxis;

    for (const auto& axis : axis) {
        auto [minProj1, maxProj1] = aCol->ProjectPolygon(aCor, axis);
        auto [minProj2, maxProj2] = bCol->ProjectPolygon(bCor, axis);

        // Calculate overlap on this axis
        float overlap = std::min(maxProj1, maxProj2) - std::max(minProj1, minProj2);
        if (overlap/10 < minOverlap) 
        {
            minOverlap = overlap/100;
            mtvAxis = axis;
        }
    }

    data.m_didCollide = true;

    // Resolve interpenetration using the minimum translation vector (MTV)
    glm::vec3 mtv = mtvAxis * (minOverlap / glm::length(mtvAxis));

    // Determine the direction to move each object
    glm::vec3 direction = aTrans->m_position - bTrans->m_position;

    //glm::normalize(direction);
    if (glm::dot(direction, mtvAxis) < 0) 
    {
        mtv = -mtv;
    }

    // Move each object by half of the MTV
    aTrans->m_position += + (mtv * 0.5f);
    bTrans->m_position += - (mtv * 0.5f);
    //Debugger::AddLog("min overlap %f", minOverlap);
    //Debugger::AddLog("mtv %f,%f,%f", mtv.x, mtv.y, mtv.z);
}

void CollisionManager::AABB_TILEMAP(GameObject aabb, GameObject tilemap, CollisionData data)
{
    // Get components
    auto aPhys = aabb.GetComponentOfType<PhysicsComponent>();
    auto aTrans = aabb.GetComponentOfType<TransformComponent>();

    // AABB position and size
    glm::vec2 aPos = aTrans->m_position;
    glm::vec2 aSize = { aTrans->m_width * aTrans->m_scale.x, aTrans->m_height * aTrans->m_scale.y };

    // Calculate bounds
    glm::vec2 a_min = aPos;
    glm::vec2 a_max = aPos + aSize;

    auto tilemapRenderer = tilemap.GetComponentOfType<TilemapRenderComponent>();
    auto grid = tilemapRenderer->GetGrid();
    auto tileData = tilemapRenderer->m_tilemapData;

    glm::vec2 bSize = { tileData.m_tileWidth * tilemapRenderer->m_transform->m_scale.x , tileData.m_tileHeight * tilemapRenderer->m_transform->m_scale.y };

    for (auto tilePos : data.m_TilePos)
    {
        // Tile position and size
        glm::vec2 bPos = tilePos;
        glm::vec2 b_min = bPos;
        glm::vec2 b_max = bPos + bSize;

        // Check for overlap
        if (a_max.x < b_min.x || a_min.x > b_max.x || a_max.y < b_min.y || a_min.y > b_max.y) {
            continue; // No collision with this tile
        }

        // Calculate overlap
        float overlapX = std::min(a_max.x - b_min.x, b_max.x - a_min.x);
        float overlapY = std::min(a_max.y - b_min.y, b_max.y - a_min.y);

        glm::vec2 collisionNormal;

        // Determine collision normal based on smallest overlap
        if (overlapX < overlapY)
        {
            if (a_min.x < b_min.x)
            {
                collisionNormal = glm::vec2(-1, 0); // AABB is to the left of the tile
            }
            else
            {
                collisionNormal = glm::vec2(1, 0); // AABB is to the right of the tile
            }
        }
        else
        {
            if (a_min.y < b_min.y)
            {
                collisionNormal = glm::vec2(0, -1); // AABB is below the tile
            }
            else
            {
                collisionNormal = glm::vec2(0, 1); // AABB is above the tile
            }
        }

        // Separate the objects
        if (overlapX < overlapY)
        {
            if (collisionNormal.x < 0)
            {
                aTrans->m_position.x -= overlapX;
                Debug::Error("" + std::to_string(overlapX));
            }
            else
            {
                aTrans->m_position.x += overlapX;
                Debug::Error(""+std::to_string(overlapX));
            }
        }
        else
        {
            if (collisionNormal.y < 0)
            {
                aTrans->m_position.y -= overlapY;
            }
            else
            {
                aTrans->m_position.y += overlapY;
            }
        }

        // Calculate relative velocity
        glm::vec2 relativeVelocity = -aPhys->GetVelocity(); // Since tilemap is static, only aabb's velocity matters
        float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

        if (velocityAlongNormal == 0)
        {
            continue; // No impact velocity along the collision normal, so skip impulse calculation
        }

        // Calculate restitution
        float e = 0.3; // Tilemap has no restitution

        // Calculate impulse scalar
        float j = -(1 + e) * velocityAlongNormal;
        j /= aPhys->getInverseMass(); // Tilemap has infinite mass, so only aabb's mass is considered

        // Apply impulse
        glm::vec2 impulse = j * collisionNormal;
        aPhys->SetVelocity(aPhys->GetVelocity() - glm::vec3((impulse * aPhys->getInverseMass()), 0));
    }
}


void CollisionManager::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
    std::string label = "Collider Manager##" + std::to_string(ID);
    if (ImGui::CollapsingHeader(label.c_str())) 
    {
        if (DebuggerFunctions::Button("Toggle Collisions")) m_colliding = !m_colliding;
        RemoveComponentMenu(ID);
    }
#endif
}