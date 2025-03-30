#include "AIGridManager.h"

void AIGridManager::InsertTileMapPrefab(std::vector<Cell>& grid, std::string path, int widthOffset, int heightOffset)
{
	std::string stringData = Resources::GetStringFromFile(path);
	std::vector<std::string> rowData;
	std::stringstream rowStringSteam(stringData);
	std::string row;
	std::string tile;


	unsigned int rowNum = 0;
	unsigned int colNum = 0;
	while (getline(rowStringSteam, row, '\n')) //split for each row
	{
		std::stringstream tileStringSteam(row);

		while (getline(tileStringSteam, tile, ',')) //now split each column to get the tile
		{
			if (rowNum > (unsigned int)m_gridHeightInCells)
			{
				return;
			}
			int tileType = std::stoi(tile);
			int a = (colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells;
			if (tileType != 0)
			{
				grid[(colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells].m_type = tileType;
			}

			colNum++;
		}

		rowNum++;
		colNum = 0;
	}
	/*for (size_t i = 0; i < grid.size(); i++)
	{
		grid[i].m_type = 2;
	}*/
}
