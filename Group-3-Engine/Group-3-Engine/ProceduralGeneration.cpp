#include "ProceduralGeneration.h"

ProceduralGeneration::ProceduralGeneration(int width, int height)
{
	//Main 4 directions (NEEDED)
	m_leftRooms.push_back({ "assets/tilemaps/LeftRoom.txt", Direction::Left, 9, 5 });
	m_rightRooms.push_back({ "assets/tilemaps/RightRoom.txt", Direction::Right, 9, 5 });
	m_upRooms.push_back({ "assets/tilemaps/UpRoom.txt", Direction::Up, 9, 5 });
	m_downRooms.push_back({ "assets/tilemaps/DownRoom.txt", Direction::Down, 9, 5 });

	m_leftRooms.push_back({ "assets/tilemaps/LeftRightRoom.txt", Direction::Right, 9, 5 });
	m_rightRooms.push_back({ "assets/tilemaps/LeftRightRoom.txt", Direction::Left, 9, 5 });
	
	m_upRooms.push_back({ "assets/tilemaps/UpDownRoom.txt", Direction::Down, 9, 5 });
	m_upRooms.push_back({ "assets/tilemaps/BigUpRoom.txt", Direction::Down, 21, 9 });
	m_upRooms.push_back({ "assets/tilemaps/BigUpDownRoom.txt", Direction::Down, 21, 9 });

	m_downRooms.push_back({ "assets/tilemaps/BigUpDownRoom.txt", Direction::Up, 21, 9 });
	m_downRooms.push_back({ "assets/tilemaps/UpRightDownRoom.txt", Direction::Up, 9, 5 });

	m_gridWidthInCells = width;
	m_gridHeightInCells = height;

	m_initRoom = true;
}

Vector2 ProceduralGeneration::FindEntrance(Room room, Direction directionToFindEntrance)
{
	for (Exit exit : room.m_exits)
	{
		if (exit.direction == directionToFindEntrance)
		{
			return Vector2(exit.x, exit.y);
		}
	}
	return Vector2(0, 0);
}

std::vector<Cell>& ProceduralGeneration::InitialiseStartingRoom(int width, int height, int maxWidth, int maxHeight)
{
	Room startRoom = { "assets/tilemaps/UpRightDownLeft.txt", Direction::None, 9, 5 };

	for (size_t x = 0; x < maxWidth; x++)
	{
		for (size_t y = 0; y < maxHeight; y++)
		{
			m_newGrid.push_back(Cell());
		}
	}

	InsertTileMapPrefab(startRoom, width, height, Direction::None);
	m_newGrid[0].m_type = 1;
	return m_newGrid;
}


void ProceduralGeneration::ProcessRooms(Room room, int colNum, int rowNum, Direction directionToBlock)
{
	// Room is current room, has exits.
	// This room needs to spawn a room at all exits bar the directionToBlock
	for (auto exit : room.m_exits)
	{
		if (exit.direction != directionToBlock)
		{
			int roomSeed = 0;
			Vector2 roomExitPos;
			int roomXPos = colNum + exit.x;
			int roomYPos = rowNum + exit.y;
			switch (exit.direction) {
			case Direction::Up:
				roomSeed = rand() % m_downRooms.size();
				roomExitPos = FindEntrance(m_downRooms[roomSeed], Direction::Down);
				InsertTileMapPrefab(m_downRooms[roomSeed], roomXPos - roomExitPos.x, roomYPos - roomExitPos.y + 1, Direction::Down);
				break;
			case Direction::Right:
				roomSeed = rand() % m_leftRooms.size();
				roomExitPos = FindEntrance(m_leftRooms[roomSeed], Direction::Left);
				InsertTileMapPrefab(m_leftRooms[roomSeed], roomXPos + roomExitPos.x + 1, roomYPos - roomExitPos.y, Direction::Left);
				break;
			case Direction::Down:
				roomSeed = rand() % m_upRooms.size();
				roomExitPos = FindEntrance(m_upRooms[roomSeed], Direction::Up);
				InsertTileMapPrefab(m_upRooms[roomSeed], roomXPos - roomExitPos.x, roomYPos - roomExitPos.y - 1, Direction::Up);
				break;
			case Direction::Left:
				roomSeed = rand() % m_rightRooms.size();
				roomExitPos = FindEntrance(m_rightRooms[roomSeed], Direction::Right);
				InsertTileMapPrefab(m_rightRooms[roomSeed], roomXPos - roomExitPos.x - 1, roomYPos - roomExitPos.y, Direction::Right);
				break;
			}
		}
	}
}

void ProceduralGeneration::InsertTileMapPrefab(Room room, int widthOffset, int heightOffset, Direction directionToBlock)
{
	// Random number generator
	/*
		This random number generator needs to be different from other random number generators as c++ works on srand(time) but if the time 
		is minimal difference (eg: same millisecond) the rand will always produce same output.
		So I analyse the "high_resolution_clock" which works in very very small differences in time.
	*/
	auto now = std::chrono::high_resolution_clock::now();
	auto a = now.time_since_epoch().count();
	std::srand(static_cast<unsigned int>(now.time_since_epoch().count()));

	std::string stringData = Resources::GetStringFromFile(room.m_roomName);
	std::vector<std::string> rowData;
	std::stringstream rowStringSteam(stringData);
	std::string row;
	std::string tile;

	std::vector<std::string> reversedRows;

	unsigned int rowNum = 0;
	unsigned int colNum = 0;

	// Read all rows into a vector, reversing their order
	while (getline(rowStringSteam, row, '\n')) {
		reversedRows.push_back(row);
	}

	for (int rowIndex = reversedRows.size() - 1; rowIndex >= 0; --rowIndex) {
		row = reversedRows[rowIndex];
		std::stringstream tileStringSteam(row);

		// Iterate through the columns from right to left
		int colIndex = 0;
		while (getline(tileStringSteam, tile, ','))
		{
			int tileType = std::stoi(tile);
			int a = (colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells;
			if (tileType != 0)
			{
				switch (tileType) {
					case 2:
						break;
					case 3:
						break;
					case 4:
						break;
					case 5:
						break;
				default:
					int a = (colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells;
					m_newGrid[(colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells].m_type = tileType;
					m_newGrid[(colNum + widthOffset) + (rowNum + heightOffset) * m_gridWidthInCells].m_isWalkable = false;
					break;
				}
			}

			colNum++;
		}

		rowNum++;
		colNum = 0;
	}

	//Analyse direction and process that while blocking the room from spawning overlapping room
	switch (room.m_direction)
	{
	case Direction::Up:
		if (directionToBlock != Direction::Up)
			ProcessRooms(room, widthOffset, heightOffset, Direction::Down);
		break;
	case Direction::Right:
		if (directionToBlock != Direction::Right)
			ProcessRooms(room, widthOffset, heightOffset, Direction::Left);
		break;
	case  Direction::Down:
		if (directionToBlock != Direction::Down)
			ProcessRooms(room, widthOffset, heightOffset, Direction::Up);
		break;
	case  Direction::Left:
		if (directionToBlock != Direction::Left)
			ProcessRooms(room, widthOffset, heightOffset, Direction::Right);
		break;
	case Direction::None:
		ProcessRooms(room, widthOffset, heightOffset, Direction::None);
		break;
	}
}
