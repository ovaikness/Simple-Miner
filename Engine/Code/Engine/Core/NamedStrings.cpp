#include "NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <algorithm>

void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	for (XmlAttribute const* attr = element.FirstAttribute(); attr != nullptr; attr = attr->Next())
	{
		m_keyValuePairs[attr->Name()] = attr->Value();
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	m_keyValuePairs[lowerKeyName] = newValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c); });
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		return entryItr->second;
	}
	return defaultValue;
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		return entryItr->second;
	}
	return defaultValue;
}

void* NamedStrings::GetValuePointer(std::string const& keyName, void* defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		void* value = (void*)atoll(entryItr->second.c_str());
		return value;
	}
	return defaultValue;
}

bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string value = entryItr->second;
		if (value.compare("true"))
		{
			return true;
		}
		else if(value.compare("false"))
		{
			return false;
		}
		return defaultValue;
	}
	return defaultValue;
}

int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		return atoi(valueAsText.c_str());
	}
	return defaultValue;
}

char NamedStrings::GetValue(std::string const& keyName, char defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		return static_cast<char>(atoi(valueAsText.c_str()));
	}
	return defaultValue;
}

float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		return static_cast<float>(atof(valueAsText.c_str()));
	}
	return defaultValue;
}

Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		Rgba8 color;
		color.SetFromText(valueAsText.c_str());
		return color;
	}
	return defaultValue;
}

EulerAngles NamedStrings::GetValue(std::string const& keyName, EulerAngles defaultValue) const
{
	std::string lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		EulerAngles angles;
		angles.SetFromText(valueAsText.c_str());
		return angles;
	}
	return defaultValue;
}

Vec3 NamedStrings::GetValue(std::string const& keyName, Vec3 defaultValue) const
{
	std::string	lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		Vec3 vec;
		vec.SetFromText(valueAsText.c_str());
		return vec;
	}
	return defaultValue;
}

Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 defaultValue) const
{
	std::string	lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		Vec2 vec;
		vec.SetFromText(valueAsText.c_str());
		return vec;
	}
	return defaultValue;
}

IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 defaultValue) const
{
	std::string	lowerKeyName = keyName;
	std::transform(lowerKeyName.begin(), lowerKeyName.end(), lowerKeyName.begin(), [](unsigned char c) -> char {return (char)::tolower(c);});
	auto entryItr = m_keyValuePairs.find(lowerKeyName);
	if (entryItr != m_keyValuePairs.end())
	{
		std::string valueAsText = entryItr->second;
		IntVec2 vec;
		vec.SetFromText(valueAsText.c_str());
		return vec;
	}
	return defaultValue;
}

