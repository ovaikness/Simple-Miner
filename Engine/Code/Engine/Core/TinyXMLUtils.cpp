#include "TinyXMLUtils.hpp"
#include <sstream>
int ParseXMLAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		return atoi(valueAsText);
	}
	return defaultValue;
}

bool ParseXMLAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		std::string valueAsString = std::string(valueAsText);
		if (valueAsString == "true")
		{
			return true;
		}
		if (valueAsString == "false")
		{
			return false;
		}
		return defaultValue;
	}
	return defaultValue;
}

wchar_t ParseXMLAttribute(XmlElement const& element, char const* attributeName, wchar_t defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		return static_cast<wchar_t>(atoi(valueAsText));
	}
	return defaultValue;
}

char ParseXMLAttribute(XmlElement const& element, char const* attributeName, char defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		char c = valueAsText[0];
		return c;
	}
	return defaultValue;
}

float ParseXMLAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		return static_cast<float>(atof(valueAsText));
	}
	return defaultValue;
}

FloatRange ParseXMLAttribute(XmlElement const& element, char const* attributeName, FloatRange defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		FloatRange range;
		range.SetFromText(valueAsText);
		return range;
	}
	return defaultValue;
}

Rgba8 ParseXMLAttribute(XmlElement const& element, char const* attributeName, Rgba8 defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		Rgba8 color;
		color.SetFromText(valueAsText);
		return color;
	}
	return defaultValue;
}

Vec3 ParseXMLAttribute(XmlElement const& element, char const* attributeName, Vec3 defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		Vec3 vec;
		vec.SetFromText(valueAsText);
		return vec;
	}
	return defaultValue;
}

EulerAngles ParseXMLAttribute(XmlElement const& element, char const* attributeName, EulerAngles defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		EulerAngles angles;
		angles.SetFromText(valueAsText);
		return angles;
	}
	return defaultValue;
}

Vec2 ParseXMLAttribute(XmlElement const& element, char const* attributeName, Vec2 defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		Vec2 vec;
		vec.SetFromText(valueAsText);
		return vec;
	}
	return defaultValue;
}

IntVec2 ParseXMLAttribute(XmlElement const& element, char const* attributeName, IntVec2 defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		IntVec2 vec;
		vec.SetFromText(valueAsText);
		return vec;
	}
	return defaultValue;
}

std::string ParseXMLAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	return ParseXMLAttribute(element,attributeName,std::string(defaultValue));
}

std::string ParseXMLAttribute(XmlElement const& element, char const* attributeName, std::string defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		return valueAsText;
	}
	return defaultValue;
}

Strings ParseXMLAttribute(XmlElement const& element, char const* attributeName, Strings defaultValue)
{
	if (char const* valueAsText = element.Attribute(attributeName))
	{
		Strings strings = SplitStringOnDelimiter(valueAsText, ',');
		return strings;
	}
	return defaultValue;
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, int value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, bool value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, wchar_t value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, char value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, float value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, FloatRange value)
{
	std::stringstream ss;
	ss << value.m_min << "~" << value.m_max;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, Rgba8 value)
{
	std::stringstream ss;
	ss << value.r << "," << value.g << "," << value.b << "," << value.a;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, Vec3 value)
{
	std::stringstream ss;
	ss << value.x << "," << value.y << "," << value.z;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, EulerAngles value)
{
	std::stringstream ss;
	ss << value.m_yaw << "," << value.m_pitch << "," << value.m_roll;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, Vec2 value)
{
	std::stringstream ss;
	ss << value.x << "," << value.y;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, IntVec2 value)
{
	std::stringstream ss;
	ss << value.x << "," << value.y;
	element->SetAttribute(attributeName, ss.str().c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, char const* value)
{
	element->SetAttribute(attributeName, value);
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, std::string value)
{
	element->SetAttribute(attributeName, value.c_str());
}

void WriteXMLAttribute(XmlElement* element, char const* attributeName, Strings value)
{
	std::string commaSeparatedStrings;
	for (std::string s : value)
	{
		commaSeparatedStrings += s + ",";
	}
	element->SetAttribute(attributeName, commaSeparatedStrings.c_str());
}
