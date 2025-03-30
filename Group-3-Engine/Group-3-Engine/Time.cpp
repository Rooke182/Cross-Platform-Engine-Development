#include "Time.h"

void to_json(nlohmann::json& json, const Time& time)
{
	//function needs to exist, but we don't really want to serialise this, so just don't write anything
}

void from_json(const nlohmann::json& json, Time& time)
{
}
