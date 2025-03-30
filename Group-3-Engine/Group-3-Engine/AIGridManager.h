#pragma once
#include "Defines.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Resources.h"
#include <string>
#include "Structs.h"

class AIGridManager
{
public:
	AIGridManager(std::vector<Cell>& grid, int width, int height) : m_grid(grid) {
		m_grid = grid;
		m_gridWidthInCells = width;
		m_gridHeightInCells = height;
	};
	void InsertTileMapPrefab(std::vector<Cell>& grid, std::string path, int widthOffset, int heightOffset);

private:
	std::vector<Cell> m_grid;
	int m_gridWidthInCells;
	int m_gridHeightInCells;

};



