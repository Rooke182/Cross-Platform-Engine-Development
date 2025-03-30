#pragma once
#include "BaseComponent.h"
#include "Defines.h"
#include <vector>
#include "Structs.h"
#include <math.h>
#include "AIGridManager.h"
#include "Vector2.h"

class NavigationComponent : public BaseComponent
{
public:
	//A* pathfinding
	friend class DebuggerMenus;
	//Width and height in cells, Size = size of cell
	void Init(int _width, int _height, float _size);
	void Update(Time time) override;

	//Current local positioning of AI, end positioning of AI (NOT IN CELLS)
	std::vector<Cell> Pathfinding(Vector2 currentPos, Vector2 endPos);
	Cell FindNearestCell(Vector2 pos);
	void ActivateNeighbouringCells(Cell cell, Cell startingCell, Cell endCell);

	float Heuristic(Vector2 pointA, Vector2 pointB);
	bool IsCellInOpenList(Cell cell);
	bool IsCellInClosedList(Cell cell);
	int GetID(Cell cell);
	float CalculateDistance(Cell cell1, Cell cell2);
	Cell ScanOpenCells();
	void RemoveFromOpenCells(Cell cell);
	Vector2 GetRandomCellPos();
	void ProcessNeighborCell(int neighborID, Cell& currentCell, Cell& startingCell, Cell& endCell);

	CreateComponentID(NavigationComponent);

	std::string GetName() override { return "NavigationComponent"; };

protected:
	//std::vector<Waypoint> m_waypointList
	AIGridManager* m_grid2;
	std::vector<Cell> m_grid;
	std::vector<Cell> m_openCells;
	std::vector<Cell> m_closedCells;
	std::vector<Cell> m_cellPath;
	float m_size;
	int m_width;
	int m_height;
};

