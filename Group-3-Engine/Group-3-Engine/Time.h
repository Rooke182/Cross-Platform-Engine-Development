#pragma once
#include <json.hpp>

struct Time
{
public:
	float m_deltaTime;
	float m_timeSinceStart;
};

void to_json(nlohmann::json& json, const Time& time);
void from_json(const nlohmann::json& json, Time& time);