#include "GlmSerialise.h"

void glm::to_json(nlohmann::json& json, const vec3& vec)
{
	json["x"] = vec.x;
	json["y"] = vec.y;
	json["z"] = vec.z;
}

void glm::from_json(const nlohmann::json& json, glm::vec3& vec)
{
	vec.x = json["x"];
	vec.y = json["y"];
	vec.z = json["z"];
}

void glm::to_json(nlohmann::json& json, const vec4& vec)
{
	json["x"] = vec.x;
	json["y"] = vec.y;
	json["z"] = vec.z;
	json["w"] = vec.w;
}

void glm::from_json(const nlohmann::json& json, vec4& vec)
{
	vec.x = json["x"];
	vec.y = json["y"];
	vec.z = json["z"];
	vec.w = json["w"];
}
