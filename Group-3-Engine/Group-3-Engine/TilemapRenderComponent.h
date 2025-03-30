#pragma once
#include "RenderComponent.h"
#include "TilemapData.h"

class Application;
class TilemapRenderComponent : public RenderComponent
{
protected:
	float m_width = 1.0f;
	float m_screenHeight = 7.28f - m_width;
	float m_tileWidth;
	float m_tileHeight;
	std::vector<Vertex> m_vertices;
	std::vector<Vertex> m_outlineVertices;
	std::vector<unsigned int> m_indices;
	friend class Debugger;
	friend class Collider;
	friend class CollisionManager;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;
	void RenderComponentMenu(int ID) override;
	TilemapData m_tilemapData;
	const int windowHeightOffset = 360;

public:
	TilemapRenderComponent(TilemapData m_tilemapData);
	TilemapRenderComponent() {};

	CreateComponentID(TilemapRenderComponent);
	SerialiseVariables(5, m_tilemapData, m_width, m_screenHeight, m_tileWidth, m_tileHeight);

	virtual void Init() override;

	Vector2 GetCellWorld(Vector2 pos);
	TilemapData GetTilemap() { return m_tilemapData; }
	std::vector<Cell>& GetGrid() { return m_tilemapData.GetGrid(); };
	void SetGrid(std::vector<Cell>& grid) { m_tilemapData.m_grid = grid; };

	std::string GetName() override { return "TilemapRenderComponent"; };
};
