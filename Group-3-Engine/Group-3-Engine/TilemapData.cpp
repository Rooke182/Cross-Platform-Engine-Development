#include "TilemapData.h"

float TilemapData::GetUVColumn(int type)
{
	return (float)((type-1) % m_tilemapColCountUV);
}

float TilemapData::GetUVRow(int type)
{
	return (float)((type-1) / m_tilemapColCountUV);
}

bool TilemapData::IsValid(Vector2 coords)
{
	if (coords.x < 0 || coords.y < 0)
		return false;
	if (coords.x >= 200 || coords.y >= 200)
		return false;
	//Passed Checks
	return true;
}

Cell TilemapData::GetTile(int x, int y)
{
	if (IsValid(Vector2(x, y)))
	{
		return m_grid[GetTileIndex(x, y)];
	}
	return {};
}
//World Pos
Cell TilemapData::GetTile(Vector2 pos)
{
	if (IsValid(pos))
	{
		return m_grid[GetTileIndex(std::floor(pos.x), std::floor(pos.y))];
	}
	return {};
}

int TilemapData::GetTileIndex(int x, int y)
{
	return x + (y * 200);
}

bool TilemapData::IsCoordsInBounds(int x, int y)
{
	if (x < 0 || y < 0)
		return false;
	if (x >= m_tilemapColCount || y >= m_tilemapRowCount)
		return false;
	return true;
}

Vector2 TilemapData::FindNearestCell(Vector2 pos)
{
	//Divide by size to get float grid coordinates
	//Then floor to get int grid coodinates
	int xCoord = static_cast<int>(std::floor(pos.x / 5));
	int yCoord = static_cast<int>(std::floor(pos.y / 5));
	
	// Retrieve cell from the grid
	return Vector2(xCoord, yCoord);
}

void TilemapData::LoadCSV(std::string path, bool resetGrid)
{
	std::string stringData = Resources::GetStringFromFile(path);
	if (stringData == "")
		return;

	m_lastCSVPath = path;
	std::vector<std::string> rowData;
	std::stringstream rowStringSteam(stringData);
	std::string row;
	std::string tile;

	if (resetGrid) //refresh grid size before loading in fully
	{
		std::stringstream rowStringSteamCheck(stringData);
		while (getline(rowStringSteamCheck, row, '\n'))
		{
			if (row.substr(0, 7) == "#gridW ")
			{

			}
			else if (row.substr(0, 7) == "#gridH ")
			{

			}
		}
	}


	int rowNum = 0;
	int colNum = 0;
	while (getline(rowStringSteam, row, '\n')) //split for each row
	{
		std::stringstream tileStringSteam(row);
		if (row.c_str()[0] != '#')
		{
			while (getline(tileStringSteam, tile, ',')) //now split each column to get the tile
			{
                if (!tile.empty() && !std::isspace(tile[0]))
                {
                    int tileType = tileType = std::stoi(tile);

                    m_grid[GetTileIndex(colNum, rowNum)].m_type = tileType;
                    //Check For Is Walkable
                    m_grid[GetTileIndex(colNum, rowNum)].m_isWalkable = false;
                }

				colNum++;
			}

			rowNum++;
			colNum = 0;
		}
		else
		{
			//load in extra data
			if (row.substr(0, 9) == "#texture ")
			{
				m_texturePath = row.substr(9);
			}
			else if (row.substr(0, 7) == "#tileW ")
			{
				m_tileWidth = std::stoi(row.substr(7));
			}
			else if (row.substr(0, 7) == "#tileH ")
			{
				m_tileHeight = std::stoi(row.substr(7));
			}
		}
	}
	CheckTexture();
	m_tilemapRowCount = rowNum;

}

void TilemapData::SaveCSV(std::string path)
{
	std::ofstream outputFile(path);
	if (!outputFile) {
		std::cerr << "Error: Unable to open file for writing: " << path << std::endl;
		return;
	}

	int lastY = 0;
	for (auto& tile : m_grid)
	{
		if (tile.m_gridY != lastY)
			outputFile << std::endl; //new line

		outputFile << tile.m_type;
		outputFile << ",";
		lastY = tile.m_gridY;
	}

	//save extra data
	outputFile << std::endl << "#texture " << m_texturePath;
	outputFile << std::endl << "#tileW " << std::to_string(m_tileWidth);
	outputFile << std::endl << "#tileH " << std::to_string(m_tileHeight);
	outputFile << std::endl << "#gridW " << std::to_string(m_gridWidthInCells);
	outputFile << std::endl << "#gridH " << std::to_string(m_gridHeightInCells);

	outputFile.close();
	std::cout << "Cells saved to file: " << path << std::endl;
}

void TilemapData::InitCells(int width, int height)
{
	m_gridWidthInCells = width;
	m_gridHeightInCells = height;

	//Goes from 0,0 to 1,0 to 2,0 etc...
	//x first then y
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//Create Grid with coords, (0,0 = bottom left, width,height = top right)
			CreateGrid(x, y);
		}
	}
}


void TilemapData::CreateGrid(int width, int height)
{
	Cell newGrid = Cell();

	//width and height = Grid Coords
	newGrid.m_gridX = width;
	newGrid.m_gridY = height;

	//width and height * size = Grid Pos
	newGrid.m_posX = (float)height;
	newGrid.m_posY = (float)height;
	//IsWalkable (self explanatory)
	newGrid.m_isWalkable = true;
	//0 out all values
	newGrid.m_fCost = 0;
	newGrid.m_gCost = 0;
	newGrid.m_hCost = 0;
	//Set to invalid cell number for pathfinding
	newGrid.m_parentCell = -1;

	newGrid.m_type = 0;

	//Add to grid vector
	m_grid.push_back(newGrid);
}

void TilemapData::CheckTexture()
{
	auto tex = Resources::GetTexture(m_texturePath);
	m_tilemapColCountUV = (unsigned int)floor((unsigned int)tex->m_width / m_tileWidth);
	m_tilemapRowCountUV = (unsigned int)floor((unsigned int)tex->m_height / m_tileHeight);
}


void TilemapData::IncreaseGridWidth()
{
	for (int i = 0; i < m_gridHeightInCells; i++)
	{
		Cell newGrid = Cell();

		//width and height = Grid Coords
		newGrid.m_gridX = m_gridWidthInCells;
		newGrid.m_gridY = i;
		//width and height * size = Grid Pos
		newGrid.m_posX = (float)i;
		newGrid.m_posY = (float)i;
		//IsWalkable (self explanatory)
		newGrid.m_isWalkable = true;
		//0 out all values
		newGrid.m_fCost = 0;
		newGrid.m_gCost = 0;
		newGrid.m_hCost = 0;
		//Set to invalid cell number for pathfinding
		newGrid.m_parentCell = -1;
		newGrid.m_type = 0;

		m_grid.insert(m_grid.begin() + (m_gridWidthInCells * (i+1)) + i, newGrid);
	}
	m_gridWidthInCells++;
}
void TilemapData::DecreaseGridWidth()
{
	if (m_gridWidthInCells == 1)
		return; //DO NOT
	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_grid[i].m_gridX == m_gridWidthInCells - 1)
		{
			m_grid.erase(m_grid.begin() + i);
			i--;
		}
	}
	m_gridWidthInCells--;
}

void TilemapData::UpdateGridWidth()
{
	for (int i = 0; i < m_gridHeightInCells; i++)
	{
		Cell newGrid = Cell();

		//width and height = Grid Coords
		newGrid.m_gridX = m_gridWidthInCells;
		newGrid.m_gridY = i;
		//width and height * size = Grid Pos
		newGrid.m_posX = (float)i;
		newGrid.m_posY = (float)i;
		//IsWalkable (self explanatory)
		newGrid.m_isWalkable = true;
		//0 out all values
		newGrid.m_fCost = 0;
		newGrid.m_gCost = 0;
		newGrid.m_hCost = 0;
		newGrid.m_parentCell = 0;
		newGrid.m_type = 0;

		m_grid.insert(m_grid.begin() + (m_gridWidthInCells * (i + 1)) + i, newGrid);
	}
}

void TilemapData::IncreaseGridHeight()
{
	for (int i = 0; i < m_gridWidthInCells; i++)
	{
		Cell newGrid = Cell();

		//width and height = Grid Coords
		newGrid.m_gridX = i;
		newGrid.m_gridY = m_gridHeightInCells;
		//width and height * size = Grid Pos
		newGrid.m_posX = (float)m_gridHeightInCells;
		newGrid.m_posY = (float)m_gridHeightInCells;
		//IsWalkable (self explanatory)
		newGrid.m_isWalkable = true;
		//0 out all values
		newGrid.m_fCost = 0;
		newGrid.m_gCost = 0;
		newGrid.m_hCost = 0;
		newGrid.m_parentCell = 0;
		newGrid.m_type = 0;
		m_grid.push_back(newGrid);
	}
	m_gridHeightInCells++;
}
void TilemapData::DecreaseGridHeight()
{
	if (m_gridHeightInCells == 1)
		return; //DO NOT

	for (int i = 0; i < m_gridWidthInCells; i++)
	{
		m_grid.erase(m_grid.begin() + m_grid.size()-1);
	}
	m_gridHeightInCells--;
}

void TilemapData::DoProcGen()
{
	procGen = new ProceduralGeneration(m_gridHeightInCells, m_gridWidthInCells);
	LoadProcGenMap();
}

void TilemapData::LoadProcGenMap() 
{
	m_procGenGrid = procGen->LoadRooms();
	for (int i = 0; i < m_procGenGrid.size(); i++)
	{
		if (m_procGenGrid[i].m_type != 0)
		{
			m_grid[i].m_type = m_procGenGrid[i].m_type;
		}
	}
}

void TilemapData::CopyOnto(TilemapData& data)
{
	data.m_grid = {};
	for (auto tile : m_grid) //intentionally create a copy
		data.m_grid.push_back(tile);
	data.m_tilemapColCount = m_tilemapColCount;
	data.m_tilemapRowCount = m_tilemapRowCount;
	data.m_tilemapColCountUV = m_tilemapColCountUV;
	data.m_tilemapRowCountUV = m_tilemapRowCountUV;
	data.m_tileWidth = m_tileWidth;
	data.m_tileHeight = m_tileHeight;
	data.m_gridWidthInCells = m_gridWidthInCells;
	data.m_gridHeightInCells = m_gridHeightInCells;
	data.m_texturePath = m_texturePath;
}

void TilemapData::FloodFillAtTile(int x, int y, int type)
{
	if (!IsCoordsInBounds(x, y))
		return;
	//https://en.wikipedia.org/wiki/Flood_fill !!!
	std::vector<Cell> nodes;
	nodes.push_back(GetTile(x, y));
	int typeToReplace = GetTile(x, y).m_type;

	while (!nodes.empty())
	{
		Cell node = nodes[0];
		nodes.erase(nodes.begin());
		if (node.m_type == typeToReplace)
		{
			int nodeX = node.m_gridX;
			int nodeY = node.m_gridY;
			
			m_grid[GetTileIndex(nodeX, nodeY)].m_type = type; //actually set the one in the grid
			if (IsCoordsInBounds(nodeX+1, nodeY))
				nodes.push_back(GetTile(nodeX+1, nodeY));
			if (IsCoordsInBounds(nodeX - 1, nodeY))
				nodes.push_back(GetTile(nodeX - 1, nodeY));
			if (IsCoordsInBounds(nodeX, nodeY + 1))
				nodes.push_back(GetTile(nodeX, nodeY + 1));
			if (IsCoordsInBounds(nodeX, nodeY - 1))
				nodes.push_back(GetTile(nodeX, nodeY - 1));
		}
	}

}

nlohmann::json TilemapData::to_json() const
{
	nlohmann::json json;

	if (m_serializeGrid)
		json["m_grid"] = m_grid;

	json["m_tilemapColCount"] = m_tilemapColCount;
	json["m_tilemapRowCount"] = m_tilemapRowCount;

	json["m_tilemapColCountUV"] = m_tilemapColCountUV;
	json["m_tilemapRowCountUV"] = m_tilemapRowCountUV;

	json["m_tileWidth"] = m_tileWidth;
	json["m_tileHeight"] = m_tileHeight;

	json["m_gridWidthInCells"] = m_gridWidthInCells;
	json["m_gridHeightInCells"] = m_gridHeightInCells;

	json["m_lastCSVPath"] = m_lastCSVPath;
	json["m_texturePath"] = m_texturePath;

	json["m_serializeGrid"] = m_serializeGrid;

	return json;
}

void TilemapData::from_json(const nlohmann::json& json)
{
    TilemapData::InitCells(200, 200); //PLEASE STOP WITH THIS SHIT

	if (json.contains("m_serializeGrid"))
		m_serializeGrid = json["m_serializeGrid"];

	if (m_serializeGrid) //make it optional
		m_grid = json["m_grid"];
	else
	{
		LoadCSV(json["m_lastCSVPath"], true);
	}


	m_tilemapColCount = json["m_tilemapColCount"];
	m_tilemapRowCount = json["m_tilemapRowCount"];

	m_tilemapColCountUV = json["m_tilemapColCountUV"];
	m_tilemapRowCountUV = json["m_tilemapRowCountUV"];

	m_tileWidth = json["m_tileWidth"];
	m_tileHeight = json["m_tileHeight"];

	m_gridWidthInCells = json["m_gridWidthInCells"];
	m_gridHeightInCells = json["m_gridHeightInCells"];

	m_lastCSVPath = json["m_lastCSVPath"];
	m_texturePath = json["m_texturePath"];
}

void to_json(nlohmann::json& json, const TilemapData& tilemapData)
{
	json = tilemapData.to_json();
}

void from_json(const nlohmann::json& json, TilemapData& tilemapData)
{
	tilemapData.from_json(json);
}
