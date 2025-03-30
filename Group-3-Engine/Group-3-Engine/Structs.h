#pragma once
#include <json.hpp>

struct Cell
{
	int m_gridX, m_gridY;
	float m_posX;
	float m_posY;
	bool m_isWalkable;
	bool m_isSelected;
	int m_parentCell;
	float m_gCost;
	float m_hCost;
	float m_fCost;

	int m_type;
};

void to_json(nlohmann::json& json, const Cell& cell);
void from_json(const nlohmann::json& json, Cell& cell);
enum BTState
{
	SUCCESS,
	FAILURE,
	PROCESSING,
};
