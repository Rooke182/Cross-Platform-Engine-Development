#include "NavigationComponent.h"
#include "TilemapRenderComponent.h"
#include "Scene.h"

void NavigationComponent::Init(int _width, int _height, float _size)
{
	m_width = _width;
	m_height = _height;
	m_grid = std::vector<Cell>();
}

void NavigationComponent::Update(Time time)
{
	//movement?
}

std::vector<Cell> NavigationComponent::Pathfinding(Vector2 currentPos, Vector2 endPos)
{
	ComponentPtr<TilemapRenderComponent> tilemapRC;
	for (GameObject gO : m_parent->GetScene()->m_gameObjects)
	{
		if (gO.GetComponentOfType<TilemapRenderComponent>() != NULL)
		{
			tilemapRC = gO.GetComponentOfType<TilemapRenderComponent>();
		}
	}

	std::vector<Cell> grid = tilemapRC->GetGrid();
	m_grid = grid;

	m_width = 200;
	m_height = 200;
	m_size = 1;

	// Clear all vectors
	m_openCells.clear();
	m_closedCells.clear();
	m_cellPath.clear();

	// Get destination and starting positions
	Cell endCell = FindNearestCell(endPos);
	if (!endCell.m_isWalkable)
		return std::vector<Cell>();
	Cell startingCell = FindNearestCell(currentPos);
	Cell currentCell = startingCell;

	// If the start and end cells are the same, return an empty path
	if (currentCell.m_gridX == endCell.m_gridX && currentCell.m_gridY == endCell.m_gridY)
		return std::vector<Cell>();

	// Initialize the start cell
	m_openCells.push_back(startingCell);

	bool foundEnd = false;

	// Main A* algorithm loop
	while (!foundEnd && !m_openCells.empty())
	{
		// Activate neighboring cells and calculate costs
		currentCell = ScanOpenCells();
		RemoveFromOpenCells(currentCell);

		// Check if the end cell is reached
		if (currentCell.m_gridX == endCell.m_gridX && currentCell.m_gridY == endCell.m_gridY)
		{
			endCell.m_parentCell = currentCell.m_parentCell;
			foundEnd = true;
		}
		else
		{
			// Add the current cell to the closed list
			m_closedCells.push_back(currentCell);

			// Activate neighboring cells
			ActivateNeighbouringCells(currentCell, startingCell, endCell);
		}
	}

	// Reconstruct the path from end to start
	if (foundEnd)
	{
		Cell tempCell = endCell;
		while (tempCell.m_parentCell != -1)
		{
			m_cellPath.push_back(tempCell);
			tempCell = m_grid[tempCell.m_parentCell];
		}

	}

	return m_cellPath;
}




Cell NavigationComponent::ScanOpenCells()
{
	float fCost = std::numeric_limits<float>::max();
	Cell lowestFCostCell;
	bool found = false;

	// Scan through all open cells and find the one with the lowest F-cost
	for (auto& cell : m_openCells)
	{
		if (cell.m_fCost < fCost || (cell.m_fCost == fCost && cell.m_hCost < lowestFCostCell.m_hCost))
		{
			lowestFCostCell = cell;
			fCost = cell.m_fCost;
			found = true;
		}
	}

	// Ensure a valid cell was found
	if (found)
	{
		return lowestFCostCell;
	}

	// Return an invalid Cell if no valid cell was found
	return Cell();
}



void NavigationComponent::ActivateNeighbouringCells(Cell cell, Cell startingCell, Cell endCell)
{
	int id = GetID(cell);

	// Up
	int upID = id + m_width;
	if (upID < (m_width * m_height))
	{
		ProcessNeighborCell(upID, cell, startingCell, endCell);
	}

	// Down
	int downID = id - m_width;
	if (downID >= 0)
	{
		ProcessNeighborCell(downID, cell, startingCell, endCell);
	}

	// Left
	int leftID = id - 1;
	if ((leftID % m_width) != (m_width - 1) && leftID >= 0)
	{
		ProcessNeighborCell(leftID, cell, startingCell, endCell);
	}

	// Right
	int rightID = id + 1;
	if ((rightID % m_width) != 0)
	{
		ProcessNeighborCell(rightID, cell, startingCell, endCell);
	}
}

void NavigationComponent::ProcessNeighborCell(int neighborID, Cell& currentCell, Cell& startingCell, Cell& endCell)
{
	Cell& neighborCell = m_grid[neighborID];

	if (neighborCell.m_isWalkable && !IsCellInClosedList(neighborCell))
	{
		float tentativeGCost = currentCell.m_gCost + 1; // assuming uniform cost of 1 for movement

		if (!IsCellInOpenList(neighborCell) || tentativeGCost < neighborCell.m_gCost)
		{
			neighborCell.m_gCost = tentativeGCost;
			neighborCell.m_hCost = Heuristic(Vector2(neighborCell.m_posX, neighborCell.m_posY), Vector2(endCell.m_posX, endCell.m_posY));
			neighborCell.m_fCost = neighborCell.m_gCost + neighborCell.m_hCost;
			neighborCell.m_parentCell = GetID(currentCell);

			if (!IsCellInOpenList(neighborCell))
			{
				m_openCells.push_back(neighborCell);
			}
		}
	}
}

bool NavigationComponent::IsCellInOpenList(Cell cell)
{
	return std::any_of(m_openCells.begin(), m_openCells.end(), [&cell](const Cell& openCell)
		{ return openCell.m_gridX == cell.m_gridX && openCell.m_gridY == cell.m_gridY; });
}

bool NavigationComponent::IsCellInClosedList(Cell cell)
{
	return std::any_of(m_closedCells.begin(), m_closedCells.end(), [&cell](const Cell& closedCell)
		{ return closedCell.m_gridX == cell.m_gridX && closedCell.m_gridY == cell.m_gridY; });
}

int NavigationComponent::GetID(Cell cell)
{
	return cell.m_gridX + cell.m_gridY * m_width;
}

float NavigationComponent::CalculateDistance(Cell cell1, Cell cell2)
{
	int dx = abs(cell2.m_gridX - cell1.m_gridX);
	int dy = abs(cell2.m_gridY - cell1.m_gridY);
	return static_cast<float>(dx + dy);
}

float NavigationComponent::Heuristic(Vector2 pointA, Vector2 pointB)
{
	return abs(pointA.x - pointB.x) + abs(pointA.y - pointB.y);
}

void NavigationComponent::RemoveFromOpenCells(Cell cell)
{
	auto it = std::remove_if(m_openCells.begin(), m_openCells.end(), [&cell](const Cell& openCell)
		{ return openCell.m_gridX == cell.m_gridX && openCell.m_gridY == cell.m_gridY; });
	m_openCells.erase(it, m_openCells.end());
	m_closedCells.push_back(cell);
}


Cell NavigationComponent::FindNearestCell(Vector2 pos)
{
	// Divide by size to get float grid coordinates
	// Then floor to get int grid coordinates
	int xCoord = static_cast<int>(floor(pos.x / m_size));
	int yCoord = static_cast<int>(floor(pos.y / m_size));

	for (auto& cell : m_grid)
	{
		if (xCoord == cell.m_gridX && yCoord == cell.m_gridY)
		{
			return cell;
		}
	}
	return Cell(); // Return an invalid cell if not found
}

Vector2 NavigationComponent::GetRandomCellPos()
{
	int size = static_cast<int>(m_grid.size());
	Cell cell = m_grid.at(rand() % size);
	Vector2 position = Vector2(cell.m_posX, cell.m_posY);
	return position;
}