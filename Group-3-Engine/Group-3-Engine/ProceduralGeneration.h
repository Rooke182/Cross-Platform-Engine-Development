#pragma once
#include <vector>
#include <string>
#include "glm/common.hpp"
#include "Structs.h"
#include "Resources.h"
#include "Vector2.h"

enum class Direction { Left, Right, Up, Down, None };
struct Exit
{
	int x;
	int y;
	Direction direction;

	Exit(int _x, int _y, Direction _direction) : x(_x), y(_y), direction(_direction) {}
};

struct Room
{
	const std::string m_roomName;
	Direction m_direction;

	std::vector<Exit> m_exits;

	int m_width;
	int m_height;

	Room(std::string _roomName, Direction _direction, int _width, int _height)
		: m_roomName(_roomName), m_direction(_direction), m_width(_width), m_height(_height) // Member initialization list
	{
		std::string stringData = Resources::GetStringFromFile(m_roomName);
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
				int tileType = std::stoi(tile);
				if (tileType != 0)
				{
					switch (tileType)
					{
					case 2:
						m_exits.emplace_back(colNum, _height - (rowNum + 1), Direction::Up);
						break;
					case 3:
						m_exits.emplace_back(colNum, _height - (rowNum + 1), Direction::Right);
						break;
					case 4:
						m_exits.emplace_back(colNum, _height - (rowNum + 1), Direction::Down);
						break;
					case 5:
						m_exits.emplace_back(colNum, _height - (rowNum + 1), Direction::Left);
						break;
					default:
						break;
					}
				}

				colNum++;
			}

			rowNum++;
			colNum = 0;
		}
	}
};

class ProceduralGeneration
{
public:
	ProceduralGeneration(int width, int height);

	Vector2 FindEntrance(Room room, Direction directionToFindEntrance);

	std::vector<Cell>& InitialiseStartingRoom(int width, int height, int maxWidth, int maxHeight);
	void ProcessRooms(Room room, int colNum, int rowNum, Direction directionToBlock);

	void InsertTileMapPrefab(Room room, int widthOffset, int heightOffset, Direction directionToBlock);
	std::vector<Cell> LoadRooms() { return m_newGrid; };

private:
	std::vector<Room> m_leftRooms;
	std::vector<Room> m_rightRooms;
	std::vector<Room> m_upRooms;
	std::vector<Room> m_downRooms;
	std::vector<Room> m_corridors;

	int m_offsetTotal;
	int m_gridWidthInCells = 0;
	int m_gridHeightInCells = 0;
	bool m_initRoom;

	std::vector<Cell> m_newGrid;
};
