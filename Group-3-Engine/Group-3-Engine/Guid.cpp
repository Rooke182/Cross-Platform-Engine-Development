#include "Guid.h"
#include <ios>
#include <sstream>

Guid::Guid():
	m_guid()
{
	for (int i = 0; i < m_guid.size(); i++)
		m_guid[i] = std::rand();
}

Guid::Guid(const std::string& str) :
	m_guid()
{
	for (int i = 0; i < str.length(); i+=2)
	{
		std::stringstream stream;
		std::string hexString = "00";
		hexString[0] = str[i];
		hexString[1] = str[i + 1];	//Each byte is two hex characters, so copy them from the string

		stream << std::hex << hexString;

		short int character;	//When reading from the stream, if we read a byte, it only gives us a nybble for some reason
		stream >> character;	//So, read into 2 bytes, and then throw that into m_guid

		m_guid[i / 2] = character;
	}
}

std::string Guid::ToString() const
{
	std::stringstream stream;
	 
	for (unsigned char character : m_guid)
	{
		if (character < 16)
			stream << '0';	//With 0-f, we want it written as 00-0F, so add the 0 manually
		stream << std::hex << (int)character;
	}

	return std::string(stream.str());
}

Guid& Guid::operator=(const Guid& other)
{
	m_guid = other.m_guid;
	return *this;
}

bool Guid::operator==(const Guid& other) const
{
	for (int i = 0; i < m_guid.size(); i++)
	{
		if (m_guid[i] != other.m_guid[i])
			return false;
	}

	return true;
}

Guid Guid::GetZeroGuid()
{
	Guid guid;

	for (int i = 0; i < guid.m_guid.size(); i++)
		guid.m_guid[i] = 0;

	return guid;
}

void to_json(nlohmann::json& json, const Guid& guid)
{
	json["guid"] = guid.ToString();
}

void from_json(const nlohmann::json& json, Guid& guid)
{
	guid = Guid((std::string)json["guid"]);
}
