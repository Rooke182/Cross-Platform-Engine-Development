#include "Structs.h"

void to_json(nlohmann::json& json, const Cell& cell)
{
	json["m_gridX"] = cell.m_gridX;
	json["m_gridY"] = cell.m_gridY;
	json["m_posX"] = cell.m_posX;
	json["m_isWalkable"] = cell.m_isWalkable;
	json["m_parentCell"] = cell.m_parentCell;
	json["m_gCost"] = cell.m_gCost;
	json["m_hCost"] = cell.m_hCost;
	json["m_fCost"] = cell.m_fCost;
	json["m_type"] = cell.m_type;
}

void from_json(const nlohmann::json& json, Cell& cell)
{
	cell.m_gridX = json["m_gridX"];
	cell.m_gridY = json["m_gridY"];
	cell.m_posX = json["m_posX"];
	cell.m_isWalkable = json["m_isWalkable"];
	cell.m_parentCell = json["m_parentCell"];
	cell.m_gCost = json["m_gCost"];
	cell.m_hCost = json["m_hCost"];
	cell.m_fCost = json["m_fCost"];
	cell.m_type = json["m_type"];
}
