#include "Vector2.h"

void to_json(nlohmann::json& json, const Vector2& vec)
{
	json["x"] = vec.x;
	json["y"] = vec.y;
}

void from_json(const nlohmann::json& json, Vector2& vec)
{
	vec.x = json["x"];
	vec.y = json["y"];
}
