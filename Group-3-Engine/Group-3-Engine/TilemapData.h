#pragma once
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "Vector2.h"
#include "Resources.h"
#include "GameObject.h"
#include "Structs.h"
#include "ProceduralGeneration.h"

class TilemapData
{
private:
	std::vector<Cell> m_grid;
	std::vector<Cell> m_procGenGrid;

	int m_tilemapColCount = 4;
	int m_tilemapRowCount = 4;

	unsigned int m_tilemapColCountUV = 3;
	unsigned int m_tilemapRowCountUV = 1;

	unsigned int m_tileWidth = 50;
	unsigned int m_tileHeight = 50;

	int m_gridWidthInCells = 200;
	int m_gridHeightInCells = 200;
	
	std::string m_lastCSVPath = "";
	std::string m_texturePath = "assets/textures/tilemap2.png";

	bool m_serializeGrid = false;

	friend class TilemapRenderComponent;
	friend class Collider;
	friend class CollisionManager;
	friend class Debugger;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;

	float GetUVColumn(int type);
	float GetUVRow(int type);
	bool IsValid(Vector2 coords);

	ProceduralGeneration* procGen;

public:
	Cell GetTile(int x, int y);
	Cell GetTile(Vector2);
	Vector2 GetTilemapDimensions() { return Vector2(m_gridWidthInCells, m_gridHeightInCells); };
	int GetTileIndex(int x, int y);
	bool IsCoordsInBounds(int x, int y);
	Vector2 FindNearestCell(Vector2 pos);

	void LoadCSV(std::string path, bool resetGrid = false);
	void SaveCSV(std::string path);

	void InitCells(int width, int height);
	void CreateGrid(int width, int height);
	void CheckTexture();
	void IncreaseGridWidth();
	void DecreaseGridWidth();
	void UpdateGridWidth();
	void IncreaseGridHeight();
	void DecreaseGridHeight();
	void DoProcGen();
	void LoadProcGenMap();
	void CopyOnto(TilemapData& data);
	void FloodFillAtTile(int x, int y, int type);
	std::vector<Cell>& GetGrid() { return m_grid; };

	nlohmann::json to_json() const;
	void from_json(const nlohmann::json& json);
};

void to_json(nlohmann::json& json, const TilemapData& tilemapData);
void from_json(const nlohmann::json& json, TilemapData& tilemapData);