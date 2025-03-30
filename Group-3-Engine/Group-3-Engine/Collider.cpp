#include "Collider.h"
#include "GameObject.h"
#include "TilemapRenderComponent.h"
#include "Scene.h"
#include "ColliderRenderComponent.h"
#include "PhysicsComponent.h"

//#define OBBAABB(ComponentPtr<Collider> obb, ComponentPtr<Collider> aabb) \ AABBOBB(ComponentPtr<Collider> aabb, ComponentPtr<Collider> obb)

void Collider::Update(Time time)
{
    if (this->GetType() == PLANE) m_parent->GetTransform()->m_pivot = glm::vec3( 0,0,0 );
    //ComputeAxes();
}


void Collider::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
    std::string label = "Collider Component##" + std::to_string(ID);
    if (ImGui::CollapsingHeader(label.c_str()))
    {
        static std::vector<std::string> colliderTypes = { "NONE", "AABB", "CIRCLE", "PLANE", "OBB", "TILEMAP"};
        static std::vector<ColliderType> colliderTypeList = { NONE, AABB, CIRCLE, PLANE, OBB, TILEMAP };
        std::string newType = ""; 
        newType = DebuggerFunctions::DropDownPicker("Type: ", "##Type: ", &colliderTypes[m_type], colliderTypes, false);
        if (newType != "")
        {
            for (int i = 0; i < colliderTypes.size(); i++)
            {
                if (newType == colliderTypes[i])
                {
                    m_type = colliderTypeList[i];
                    if (Debugger::m_gameobjectSelected->GetComponentOfType<ColliderRenderComponent>() != nullptr)
                    {
                        auto renderer = Debugger::m_gameobjectSelected->GetComponentOfType<ColliderRenderComponent>();
                        renderer->Cleanup();
                        renderer->Init();
                    }
                    break;
                }
            }
        }

        if (this->m_axis.size() > 0)
        {
            DebuggerFunctions::Vec3Input("Axis 1:", "##Axis 1:", this->m_axis[0], true, true, true);
            DebuggerFunctions::Vec3Input("Axis 2:", "##Axis 2:", this->m_axis[1], true, true, true);
            DebuggerFunctions::Vec3Input("Normal:", "##Normal:", this->m_normal, true, true, true);
            DebuggerFunctions::Vec3Input("Top Left:", "##Top Left:", this->m_vertices[1], true, true, true);
            DebuggerFunctions::Vec3Input("Top Right:", "##Top Right:", this->m_vertices[0], true, true, true);
            DebuggerFunctions::Vec3Input("Bottom Left:", "##Bottom Left:", this->m_vertices[2], true, true, true);
            DebuggerFunctions::Vec3Input("Bottom Right:", "##Bottom Right:", this->m_vertices[3], true, true, true);
        }
        //DebuggerFunctions::Vec3Input("transform matrix", "##matrix:")

        auto transform = this->m_parent->GetTransform()->GetTransformMatrix();

        RemoveComponentMenu(ID);
    }
#endif
}


CollisionData Collider::AABB_AABB(ComponentPtr<Collider> a, ComponentPtr<Collider> b)
{
    glm::vec2 aMin = a->GetMin();
    glm::vec2 aMax = a->GetMax();
    glm::vec2 bMin = b->GetMin();
    glm::vec2 bMax = b->GetMax();

    bool xIntersection = aMin.x <= bMax.x && aMax.x >= bMin.x;
    bool yIntersection = aMin.y <= bMax.y && aMax.y >= bMin.y;

    CollisionData data{};
    data.m_didCollide = (xIntersection && yIntersection);

    if (!data.m_didCollide)
    {
        data.m_collisionNorm = glm::vec3(0.0f, 0.0f, 0.0f);
        return data;
    }

    float overlapX = std::min(aMax.x - bMin.x, bMax.x - aMin.x);
    float overlapY = std::min(aMax.y - bMin.y, bMax.y - aMin.y);


    if (overlapX < overlapY) {
        if (aMax.x > bMin.x && aMin.x < bMin.x)
        {
            data.m_collisionNorm = glm::vec3(-1.0f, 0.0f, 0.0f); // A is to the left of B
        }
        else
        {
            data.m_collisionNorm = glm::vec3(1.0f, 0.0f, 0.0f); // A is to the right of B
        }
    }
    else
    {
        if (aMax.y > bMin.y && aMin.y < bMin.y)
        {
            data.m_collisionNorm = glm::vec3(0.0f, -1.0f, 0.0f); // A is below B
        }
        else
        {
            data.m_collisionNorm = glm::vec3(0.0f, 1.0f, 0.0f); // A is above B
        }
    }

    return data;
}

CollisionData Collider::AABB_CIRCLE(ComponentPtr<Collider> aabb, ComponentPtr<Collider> circle)
{
    auto aTrans = aabb->m_parent->GetTransform();
    auto bTrans = circle->m_parent->GetTransform();

    glm::vec3 aPos = aTrans->m_position;
    glm::vec3 aSize = { aTrans->m_width, aTrans->m_height, 0 };

    glm::vec3 aMin = aPos;
    glm::vec3 aMax = aPos + aSize;

    float bRadius = (bTrans->m_width * bTrans->m_scale.x) * 0.5f;
    glm::vec3 bPos = bTrans->m_position;

    bPos.x += bRadius;
    bPos.y += bRadius;

    glm::vec3 closestPoint;
    closestPoint.x = std::max(aMin.x, std::min(bPos.x, aMax.x));
    closestPoint.y = std::max(aMin.y, std::min(bPos.y, aMax.y));
    closestPoint.z = 0;

    glm::vec3 distance = closestPoint - bPos;
    float distanceSquared = glm::dot(distance, distance);
    float radiusSquared = bRadius * bRadius;

    CollisionData data;
    if (distanceSquared < radiusSquared) data.m_didCollide = true;
    else data.m_didCollide = false;
    return data;
}

CollisionData Collider::AABB_PLANE(ComponentPtr<Collider> aabb, ComponentPtr<Collider> plane)
{
    glm::vec3 planeNormal = plane->GetRotatedNormals();

    float planeDistance = -glm::dot(planeNormal, plane->GetPosition());

    glm::vec3 aabbMin = aabb->GetMin();
    glm::vec3 aabbMax = aabb->GetMax();

    glm::vec3 vertices[8] = {
        {aabbMin.x, aabbMin.y, aabbMin.z},
        {aabbMax.x, aabbMin.y, aabbMin.z},
        {aabbMin.x, aabbMax.y, aabbMin.z},
        {aabbMax.x, aabbMax.y, aabbMin.z},
        {aabbMin.x, aabbMin.y, aabbMax.z},
        {aabbMax.x, aabbMin.y, aabbMax.z},
        {aabbMin.x, aabbMax.y, aabbMax.z},
        {aabbMax.x, aabbMax.y, aabbMax.z}
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

    CollisionData data{};
    data.m_didCollide = hasPositive&& hasNegative;
    return data;
}

CollisionData Collider::CIRCLE_PLANE(ComponentPtr<Collider> circle, ComponentPtr<Collider> plane)
{
    glm::vec3 planeNormal = plane->GetRotatedNormals();
    float planeDistance = -glm::dot(planeNormal, plane->GetPosition());

    glm::vec3 circleCenter = circle->GetCentre();
    float circleRadius = circle->GetExtents().x;

    float distanceToPlane = glm::abs(glm::dot(planeNormal, circleCenter) + planeDistance) / glm::length(planeNormal);

    CollisionData data;
    data.m_didCollide = (distanceToPlane <= circleRadius);
    return data;
}

CollisionData Collider::AABB_OBB(ComponentPtr<Collider> aabb, ComponentPtr<Collider> obb)
{
    CollisionData data{};

    std::vector<glm::vec3> axis = {};
 
    auto vertices1 = aabb->GetCorners();
    
    auto vertices2 = obb->GetCorners();
    obb->m_axis.clear();
    obb->m_axis.push_back(glm::vec3(vertices2[1].x - vertices2[0].x, vertices2[1].y - vertices2[0].y, 0));
    obb->m_axis.push_back(glm::vec3(vertices2[2].x - vertices2[1].x, vertices2[2].y - vertices2[1].y, 0));

    //global axis for aabb
    axis.push_back(glm::vec3(1,0,0));//aabb->m_axis[1, 0, 0]);
    axis.push_back(glm::vec3(0, 1, 0));// aabb->m_axis[0, 1, 0]);

    //local axis for obb
    axis.push_back(obb->m_axis[0]);
    axis.push_back(obb->m_axis[1]);

    for (const auto& axis : axis) {
        auto [minProj1, maxProj1] = aabb->ProjectPolygon(vertices1, axis);
        auto [minProj2, maxProj2] = obb->ProjectPolygon(vertices2, axis);

        if (!obb->Overlap(minProj1, maxProj1, minProj2, maxProj2)) {
            data.m_didCollide = false; // Separating found
            return data;
        }
    }

    data.m_didCollide = true;//(aabb->AABBOBB(aabb, obb));
    return data;
}
CollisionData Collider::OBB_AABB(ComponentPtr<Collider> obb, ComponentPtr<Collider> aabb)
{
    CollisionData data{};

    std::vector<glm::vec3> axis;

    auto vertices2 = aabb->GetCorners();

    auto vertices1 = obb->GetCorners();
    obb->m_axis.clear();
    obb->m_axis.push_back(glm::vec3(vertices1[1].x - vertices1[0].x, vertices1[1].y - vertices1[0].y, 0));
    obb->m_axis.push_back(glm::vec3(vertices1[2].x - vertices1[1].x, vertices1[2].y - vertices1[1].y, 0));

    //global axis for aabb
    axis.push_back(glm::vec3(1, 0, 0));//aabb->m_axis[1, 0, 0]);
    axis.push_back(glm::vec3(0, 1, 0));// aabb->m_axis[0, 1, 0]);

    //local axis for obb
    axis.push_back(obb->m_axis[0]);
    axis.push_back(obb->m_axis[1]);

    for (const auto& axis : axis) {
        auto [minProj1, maxProj1] = obb->ProjectPolygon(vertices1, axis);
        auto [minProj2, maxProj2] = aabb->ProjectPolygon(vertices2, axis);

        if (!obb->Overlap(minProj1, maxProj1, minProj2, maxProj2)) {
            data.m_didCollide = false; // Separating found
            return data;
        }
    }

    data.m_didCollide = true;//(aabb->AABBOBB(aabb, obb));
    return data;
}

CollisionData Collider::CIRCLE_CIRCLE(ComponentPtr<Collider> a, ComponentPtr<Collider> b)
{
    auto aTrans = a->m_parent->GetTransform();
    auto bTrans = b->m_parent->GetTransform();

    auto aRad = aTrans->m_width * aTrans->m_scale.x * 0.5;
    auto bRad = bTrans->m_width * bTrans->m_scale.x * 0.5;

    glm::vec2 circleDist = (glm::vec2(a->GetPosition())+glm::vec2(aRad, aRad)) - (glm::vec2(b->GetPosition()) + glm::vec2(bRad, bRad));
    float combinedRadi = aRad+bRad;
    float dist = (circleDist.x * circleDist.x + circleDist.y * circleDist.y);

    CollisionData data{};
    data.m_didCollide = dist <= combinedRadi * combinedRadi;
    return data;

}

CollisionData Collider::OBB_OBB(ComponentPtr<Collider> a, ComponentPtr<Collider> b)
{
    CollisionData data{};

    std::vector<glm::vec3> axis;

    auto vertices1 = a->GetCorners();
    a->m_axis.clear();
    a->m_axis.push_back(glm::vec3(vertices1[1].x - vertices1[0].x, vertices1[1].y - vertices1[0].y, 0));
    a->m_axis.push_back(glm::vec3(vertices1[2].x - vertices1[1].x, vertices1[2].y - vertices1[1].y, 0));

    auto vertices2 = b->GetCorners();
    b->m_axis.clear();
    b->m_axis.push_back(glm::vec3(vertices2[1].x - vertices2[0].x, vertices2[1].y - vertices2[0].y, 0));
    b->m_axis.push_back(glm::vec3(vertices2[2].x - vertices2[1].x, vertices2[2].y - vertices2[1].y, 0));

    axis.push_back(a->m_axis[0]);
    axis.push_back(a->m_axis[1]);
    axis.push_back(b->m_axis[0]);
    axis.push_back(b->m_axis[1]);

    for (const auto& axis : axis) {
        auto [minProj1, maxProj1] = a->ProjectPolygon(vertices1, axis);
        auto [minProj2, maxProj2] = a->ProjectPolygon(vertices2, axis);

        if (!a->Overlap(minProj1, maxProj1, minProj2, maxProj2)) {
            data.m_didCollide = false; // Separating found
            return data;
        }
    }

    data.m_didCollide = true;
    return data;
   
}

CollisionData Collider::TILEMAP_AABB(ComponentPtr<Collider> tilemap, ComponentPtr<Collider> aabb)
{
    auto tilemapRenderer = tilemap->m_parent->GetComponentOfType<TilemapRenderComponent>();
    CollisionData data{};

    if (tilemapRenderer != nullptr)
    {
        //Gets the entire TileMap (fix this)
        auto grid = tilemapRenderer->GetGrid();
        auto tileData = tilemapRenderer->m_tilemapData;
        float x = (aabb->GetPosition().x - tilemap->GetPosition().x) / tilemapRenderer->m_transform->m_scale.x;
        float y = (aabb->GetPosition().y - tilemap->GetPosition().y) / tilemapRenderer->m_transform->m_scale.y;
        float maxx = (aabb->GetMax().x - tilemap->GetPosition().x) / tilemapRenderer->m_transform->m_scale.x;
        float maxy = (aabb->GetMax().y - tilemap->GetPosition().y) / tilemapRenderer->m_transform->m_scale.y;

        //float center = GetTileWorld(x,y,tilemap);
        std::vector <Cell> SurroundingTile;


        glm::vec2 TileCollisionDatatopLeft = GetTileWorld(x, y, tilemap, tileData);
        glm::vec2 TileCollisionDatatopRight = GetTileWorld(maxx, y, tilemap, tileData);
        glm::vec2 TileCollisionDatabottomLeft = GetTileWorld(x, maxy, tilemap, tileData);
        glm::vec2 TileCollisionDatabottomRight = GetTileWorld(maxx, maxy, tilemap, tileData);
        float tileXpostopLeft = TileCollisionDatatopLeft.x;
        float tileYpostopLeft = TileCollisionDatatopLeft.y;

        float tileXpostopRight = TileCollisionDatatopRight.x;
        float tileYpostopRight = TileCollisionDatatopRight.y;

        float tileXposbottomLeft = TileCollisionDatabottomLeft.x;
        float tileYposbottomLeft = TileCollisionDatabottomLeft.y;

        float tileXposbottomRight = TileCollisionDatabottomRight.x;
        float tileYposbottomRight = TileCollisionDatabottomRight.y;


        auto topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft, tileYpostopLeft);
        if (topLeft.m_gridX == tileXpostopLeft && topLeft.m_gridY == tileYpostopLeft) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft - 1, tileYpostopLeft + 1);
        if (topLeft.m_gridX == tileXpostopLeft - 1 && topLeft.m_gridY == tileYpostopLeft + 1) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft, tileYpostopLeft + 1);
        if (topLeft.m_gridX == tileXpostopLeft && topLeft.m_gridY == tileYpostopLeft + 1) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft + 1, tileYpostopLeft + 1);
        if (topLeft.m_gridX == tileXpostopLeft + 1 && topLeft.m_gridY == tileYpostopLeft + 1) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft - 1, tileYpostopLeft);
        if (topLeft.m_gridX == tileXpostopLeft - 1 && topLeft.m_gridY == tileYpostopLeft) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft + 1, tileYpostopLeft);
        if (topLeft.m_gridX == tileXpostopLeft + 1 && topLeft.m_gridY == tileYpostopLeft) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft - 1, tileYpostopLeft - 1);
        if (topLeft.m_gridX == tileXpostopLeft - 1 && topLeft.m_gridY == tileYpostopLeft - 1) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft, tileYpostopLeft - 1);
        if (topLeft.m_gridX == tileXpostopLeft && topLeft.m_gridY == tileYpostopLeft - 1) SurroundingTile.push_back(topLeft);
        topLeft = tilemapRenderer->m_tilemapData.GetTile(tileXpostopLeft + 1, tileYpostopLeft - 1);
        if (topLeft.m_gridX == tileXpostopLeft + 1 && topLeft.m_gridY == tileYpostopLeft - 1) SurroundingTile.push_back(topLeft);

        auto topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight, tileYpostopRight);
        if (topRight.m_gridX == tileXpostopRight && topRight.m_gridY == tileYpostopRight) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight - 1, tileYpostopRight + 1);
        if (topRight.m_gridX == tileXpostopRight - 1 && topRight.m_gridY == tileYpostopRight + 1) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight, tileYpostopRight + 1);
        if (topRight.m_gridX == tileXpostopRight && topRight.m_gridY == tileYpostopRight + 1) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight + 1, tileYpostopRight + 1);
        if (topRight.m_gridX == tileXpostopRight + 1 && topRight.m_gridY == tileYpostopRight + 1) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight - 1, tileYpostopRight);
        if (topRight.m_gridX == tileXpostopRight - 1 && topRight.m_gridY == tileYpostopRight) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight + 1, tileYpostopRight);
        if (topRight.m_gridX == tileXpostopRight + 1 && topRight.m_gridY == tileYpostopRight) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight - 1, tileYpostopRight - 1);
        if (topRight.m_gridX == tileXpostopRight - 1 && topRight.m_gridY == tileYpostopRight - 1) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight, tileYpostopRight - 1);
        if (topRight.m_gridX == tileXpostopRight && topRight.m_gridY == tileYpostopRight - 1) SurroundingTile.push_back(topRight);
        topRight = tilemapRenderer->m_tilemapData.GetTile(tileXpostopRight + 1, tileYpostopRight - 1);
        if (topRight.m_gridX == tileXpostopRight + 1 && topRight.m_gridY == tileYpostopRight - 1) SurroundingTile.push_back(topRight);

        auto bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft, tileYposbottomLeft);
        if (bottomLeft.m_gridX == tileXposbottomLeft && bottomLeft.m_gridY == tileYposbottomLeft) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft - 1, tileYposbottomLeft + 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft - 1 && bottomLeft.m_gridY == tileYposbottomLeft + 1) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft, tileYposbottomLeft + 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft && bottomLeft.m_gridY == tileYposbottomLeft + 1) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft + 1, tileYposbottomLeft + 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft + 1 && bottomLeft.m_gridY == tileYposbottomLeft + 1) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft - 1, tileYposbottomLeft);
        if (bottomLeft.m_gridX == tileXposbottomLeft - 1 && bottomLeft.m_gridY == tileYposbottomLeft) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft + 1, tileYposbottomLeft);
        if (bottomLeft.m_gridX == tileXposbottomLeft + 1 && bottomLeft.m_gridY == tileYposbottomLeft) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft - 1, tileYposbottomLeft - 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft - 1 && bottomLeft.m_gridY == tileYposbottomLeft - 1) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft, tileYposbottomLeft - 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft && bottomLeft.m_gridY == tileYposbottomLeft - 1) SurroundingTile.push_back(bottomLeft);
        bottomLeft = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomLeft + 1, tileYposbottomLeft - 1);
        if (bottomLeft.m_gridX == tileXposbottomLeft + 1 && bottomLeft.m_gridY == tileYposbottomLeft - 1) SurroundingTile.push_back(bottomLeft);

        auto bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight, tileYposbottomRight);
        if (bottomRight.m_gridX == tileXposbottomRight && bottomRight.m_gridY == tileYposbottomRight) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight - 1, tileYposbottomRight + 1);
        if (bottomRight.m_gridX == tileXposbottomRight - 1 && bottomRight.m_gridY == tileYposbottomRight + 1) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight, tileYposbottomRight + 1);
        if (bottomRight.m_gridX == tileXposbottomRight && bottomRight.m_gridY == tileYposbottomRight + 1) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight + 1, tileYposbottomRight + 1);
        if (bottomRight.m_gridX == tileXposbottomRight + 1 && bottomRight.m_gridY == tileYposbottomRight + 1) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight - 1, tileYposbottomRight);
        if (bottomRight.m_gridX == tileXposbottomRight - 1 && bottomRight.m_gridY == tileYposbottomRight) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight + 1, tileYposbottomRight);
        if (bottomRight.m_gridX == tileXposbottomRight + 1 && bottomRight.m_gridY == tileYposbottomRight) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight - 1, tileYposbottomRight - 1);
        if (bottomRight.m_gridX == tileXposbottomRight - 1 && bottomRight.m_gridY == tileYposbottomRight - 1) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight, tileYposbottomRight - 1);
        if (bottomRight.m_gridX == tileXposbottomRight && bottomRight.m_gridY == tileYposbottomRight - 1) SurroundingTile.push_back(bottomRight);
        bottomRight = tilemapRenderer->m_tilemapData.GetTile(tileXposbottomRight + 1, tileYposbottomRight - 1);
        if (bottomRight.m_gridX == tileXposbottomRight + 1 && bottomRight.m_gridY == tileYposbottomRight - 1) SurroundingTile.push_back(bottomRight);

        for (int i = 0; i < SurroundingTile.size(); i++)
        {
            auto tileTest = SurroundingTile[i];
            if (tileTest.m_type != 0)
            {
                
                glm::vec2 tileSize = { tileData.m_tileWidth * tilemapRenderer->m_transform->m_scale.x , tileData.m_tileHeight * tilemapRenderer->m_transform->m_scale.y };
                glm::vec2 tilePosition = { tilemapRenderer->m_transform->m_position.x + (tileTest.m_gridX * tileSize.x), tilemapRenderer->m_transform->m_position.y + (tileTest.m_gridY * tileSize.y) };
                CollisionData singleTileData;
                singleTileData = TILE_AABB(tilePosition, tileSize, aabb);
                if (singleTileData.m_didCollide)
                {
                    data.m_TilePos.push_back(tilePosition);
                    //return data;
                }
            }
        }
    }
    if (data.m_TilePos.size() > 0) data.m_didCollide = true;
    return data;
}

glm::vec2 Collider::GetTileWorld(float x, float y, ComponentPtr<Collider> tilemap, TilemapData tileMapData)
{
    int tileX = floor(x / tileMapData.m_tileWidth);
    int tileY = floor(y / tileMapData.m_tileHeight);

    return glm::vec2(tileX, tileY);
}

CollisionData Collider::TILE_AABB(glm::vec2 tilePosition, glm::vec2 tileSize, ComponentPtr<Collider> aabb)
{
    glm::vec2 bMin = aabb->GetMin();
    glm::vec2 bMax = aabb->GetMax();
    glm::vec2 aMin = { tilePosition.x, tilePosition.y };
    glm::vec2 aMax = { tilePosition.x + tileSize.x ,tilePosition.y + tileSize.y };
    //Debugger::AddLog("aMin: x:%f  y:%f", aMin.x, aMin.y);
    //Debugger::AddLog("aMax: x:%f  y:%f", aMax.x, aMax.y);
    //Debugger::AddLog("bMin: x:%f  y:%f", bMin.x, bMin.y);
    //Debugger::AddLog("bMax: x:%f  y:%f", bMax.x, bMax.y);


    bool xIntersection = aMin.x <= bMax.x && aMax.x >= bMin.x;
    bool yIntersection = aMin.y <= bMax.y && aMax.y >= bMin.y;

    CollisionData data{};
    data.m_didCollide = (xIntersection && yIntersection);

    if (!data.m_didCollide)
    {
        data.m_collisionNorm = glm::vec3(0.0f, 0.0f, 0.0f);
        return data;
    }

    float overlapX = std::min(aMax.x - bMin.x, bMax.x - aMin.x);
    float overlapY = std::min(aMax.y - bMin.y, bMax.y - aMin.y);

    if (overlapX < overlapY) {
        if (aMax.x > bMin.x && aMin.x < bMin.x)
        {
            data.m_collisionNorm = glm::vec3(-1.0f, 0.0f, 0.0f); // A is to the left of B
        }
        else
        {
            data.m_collisionNorm = glm::vec3(1.0f, 0.0f, 0.0f); // A is to the right of B
        }
    }
    else
    {
        if (aMax.y > bMin.y && aMin.y < bMin.y)
        {
            data.m_collisionNorm = glm::vec3(0.0f, -1.0f, 0.0f); // A is below B
        }
        else
        {
            data.m_collisionNorm = glm::vec3(0.0f, 1.0f, 0.0f); // A is above B
        }
    }
    return data;
}

//this gets passed into the collisionManager to determine which way around the collider types are
CollisionData Collider::CollidesWith(ComponentPtr<Collider> other)
{
    switch (m_type)
    {
    case AABB:
        switch (other->m_type)
            {
            case AABB:
                return AABB_AABB(this, other);
                break;

            case CIRCLE:
                return AABB_CIRCLE(this, other);
                break;

            case PLANE:
                return AABB_PLANE(this, other); 
                break;

            case OBB:
                return AABB_OBB(this, other);
                break;

            case TILEMAP:
                return TILEMAP_AABB(other, this);
                break;
        }
        break;

    case CIRCLE:
        switch (other->m_type)
        {
        case AABB:
            return AABB_CIRCLE(other,this);
            break;

        case CIRCLE:
            return CIRCLE_CIRCLE(this, other);
            break;

        case PLANE:
            return CIRCLE_PLANE(this, other);
            break;
        case OBB:

            break;
        }
        break;

    case PLANE:
        switch (other->m_type)
        {
        case AABB:
            return AABB_PLANE(other, this);
            break;

        case CIRCLE:
            return CIRCLE_PLANE(other, this);
            break;

        case PLANE:

            break;
        case OBB:

            break;
        }
        break;
    case OBB:
        switch (other->m_type)
        {
        case AABB:
            return AABB_OBB(other, this);
            break;

        case CIRCLE:

            break;

        case PLANE:

            break;
        case OBB:
            return OBB_OBB(this, other);
            break;
        }
        break;

    case TILEMAP:
        switch (other->m_type)
        {
        case AABB:
            return TILEMAP_AABB(this, other);
            break;
        }
        break;
    }
    return {};
}


glm::vec3 Collider::GetPosition()
{
    return this->m_parent->GetTransform()->m_position;
}

glm::vec3 Collider::GetAngle()
{
    return this->m_parent->GetTransform()->m_angle;
}

glm::vec3 Collider::GetMin()
{
    return glm::vec3(glm::vec2(this->m_parent->GetTransform()->m_position),0);
}

glm::vec3 Collider::GetMax()
{
    return glm::vec3(glm::vec2(this->m_parent->GetTransform()->m_position),0) + (this->m_parent->GetTransform()->m_scale) * glm::vec3{this->m_parent->GetTransform()->m_width, this->m_parent->GetTransform()->m_height, 1.0};
}

glm::vec3 Collider::GetExtents()
{
    return glm::vec3((GetMax() - GetMin()) * 0.5f);
}

glm::vec3 Collider::GetCentre()
{
    return this->m_parent->GetTransform()->m_position + (this->m_parent->GetTransform()->m_scale * this->m_parent->GetTransform()->m_pivot * 
        glm::vec3{this->m_parent->GetTransform()->m_width, this->m_parent->GetTransform()->m_height, 1.0});
}

glm::vec3 Collider::AABBClosestPoint(glm::vec3 point)
{
    glm::vec3 res = point;
    glm::vec3 m_min = GetMin();
    glm::vec3 m_max = GetMax();

    res.x = (res.x < m_min.x) ? m_min.x : res.x;
    res.y = (res.y < m_min.y) ? m_min.y : res.y;
    res.z = (res.z < m_min.z) ? m_min.z : res.z;

    res.x = (res.x > m_max.x) ? m_max.x : res.x;
    res.y = (res.y > m_max.y) ? m_max.y : res.y;
    res.z = (res.z > m_max.z) ? m_max.z : res.z;

    return res;
}

inline glm::vec3 Collider::GetRotatedNormals()
{
    float angleRadians = glm::radians(this->GetAngle().z);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0, 0, 1));
    return  glm::vec3(rotationMatrix * glm::vec4(this->GetNormals(), 1.0f));
}

inline IntervalResult Collider::GetInterval(glm::vec3 axis)
{
    IntervalResult m_result;

    glm::vec3 m_min = this->GetMin();
    glm::vec3 m_max = this->GetMax();

    glm::vec3 verts[] =
    {
        glm::vec3(m_min.x, m_min.y, 0.0f), glm::vec3(m_min.x, m_max.y, 0.0f),
        glm::vec3(m_max.x, m_max.y, 0.0f), glm::vec3(m_max.x, m_min.y, 0.0f)
    };

    m_result.m_min = m_result.m_max = glm::dot(axis, verts[0]);

    for (int i = 0; i < 4; i++)
    {
        float m_proj = glm::dot(axis, verts[i]);
        if (m_proj < m_result.m_min)
        {
            m_result.m_min = m_proj;
        }
        if (m_proj > m_result.m_max)
        {
            m_result.m_max = m_proj;
        }
    }
    return m_result;
}

bool Collider::linePoint(float x1, float y1, float x2, float y2, float px, float py)
{
     // get distance from the point to the two ends of the line
     float d1 = (px, py, x1, y1);
     float d2 = (px, py, x2, y2);

     // get the length of the line
     float lineLen = (x1, y1, x2, y2);

     float buffer = 0.1;
     if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
         return true;
     }
     return false;
}

inline bool Collider::OverlapOnAxis(ComponentPtr<Collider> other, glm::vec3 axis)
{
    IntervalResult a = GetInterval(axis);
    IntervalResult b = other->GetInterval(axis);

    return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

inline std::vector<glm::vec4> Collider::GetCorners()
{
    m_vertices.clear();
    if (this->m_parent->GetRenderer() == nullptr) return m_vertices;
    auto renderer = this->m_parent->GetRenderer()->GetRenderData();
    auto transform = this->m_parent->GetTransform()->GetTransformMatrix();
    glm::vec4 topRight = transform * glm::vec4(renderer->m_vertexMaxX, renderer->m_vertexMinY, 0.0f, 1.0f);
    glm::vec4 topLeft = transform * glm::vec4(renderer->m_vertexMinX, renderer->m_vertexMinY, 0.0f, 1.0f);
    glm::vec4 bottomLeft = transform * glm::vec4(renderer->m_vertexMinX, renderer->m_vertexMaxY, 1.0f, 1.0f);
    glm::vec4 bottomRight = transform * glm::vec4(renderer->m_vertexMaxX, renderer->m_vertexMaxY, 1.0f, 1.0f);

    m_vertices.push_back(topRight);
    m_vertices.push_back(topLeft);
    m_vertices.push_back(bottomLeft);
    m_vertices.push_back(bottomRight);

    return m_vertices;
}

inline std::pair<float, float> Collider::ProjectPolygon(const std::vector<glm::vec4>& vertices, const glm::vec3& axis) {
    float minProjection = std::numeric_limits<float>::max();
    float maxProjection = -std::numeric_limits<float>::max();

    for (const auto& vertex : vertices) {
        float projection = vertex.x * axis.x + vertex.y * axis.y;
        minProjection = std::min(minProjection, projection);
        maxProjection = std::max(maxProjection, projection);
    }

    return { minProjection, maxProjection };
}

inline bool Collider::Overlap(float minA, float maxA, float minB, float maxB) {
    return maxA >= minB && maxB >= minA;
}

void Collider::Init()
{
    if (m_parent->GetComponentOfType<PhysicsComponent>() == nullptr)
    {
        auto phys = this->m_parent->GetScene()->MakeComponent<PhysicsComponent>();
        this->m_parent->AddComponent(phys);
    }
}