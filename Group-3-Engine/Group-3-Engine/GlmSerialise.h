#pragma once
#include <json.hpp>
#include <glm/glm.hpp>

namespace glm
{
	void to_json(nlohmann::json& json, const vec3& vec);
	void from_json(const nlohmann::json& json, vec3& vec);

	void to_json(nlohmann::json& json, const vec4& vec);
	void from_json(const nlohmann::json& json, vec4& vec);
}