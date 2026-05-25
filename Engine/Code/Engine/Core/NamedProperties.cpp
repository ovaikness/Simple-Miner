#include "Engine/Core/NamedProperties.hpp"

NamedProperties::NamedProperties()
{
}

NamedProperties::NamedProperties(NamedProperties const& copy)
{
	for (auto& entry : copy.m_Properties)
	{
		PropertyBase* property = entry.second;
		if (property)
		{
			PropertyBase* newProperty = property->Clone();
			m_Properties[entry.first] = newProperty;
		}
	}
}

NamedProperties::NamedProperties(NamedProperties&& move)
{
	m_Properties = std::move(move.m_Properties);
	move.m_Properties.clear();
}

NamedProperties& NamedProperties::operator=(NamedProperties&& move)
{
	if (this != &move)
	{
		for (auto& entry : m_Properties)
		{
			delete entry.second;
		}
		m_Properties.clear();
		m_Properties = std::move(move.m_Properties);
		move.m_Properties.clear();
	}
	return *this;
}

NamedProperties& NamedProperties::operator=(NamedProperties const& copy)
{
	if (this != &copy)
	{
		for (auto& entry : m_Properties)
		{
			delete entry.second;
		}
		m_Properties.clear();
		for (auto& entry : copy.m_Properties)
		{
			PropertyBase* property = entry.second;
			if (property)
			{
				PropertyBase* newProperty = property->Clone();
				m_Properties[entry.first] = newProperty;
			}
		}
	}
	return *this;
}

NamedProperties::~NamedProperties()
{
	for (auto& entry : m_Properties)
	{
		delete entry.second;
	}
	m_Properties.clear();
}

void NamedProperties::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	for (XmlAttribute const* attr = element.FirstAttribute(); attr != nullptr; attr = attr->Next())
	{
		SetValue(attr->Name(), attr->Value());
	}
}

void NamedProperties::SetValue(std::string const& keyName, std::string const& newValue)
{
	CreateOrAssignProperty<std::string>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, char newValue)
{
	CreateOrAssignProperty<char>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, void* newValue)
{
	CreateOrAssignProperty<void*>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, bool newValue)
{
	CreateOrAssignProperty<bool>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, int newValue)
{
	CreateOrAssignProperty<int>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, float newValue)
{
	CreateOrAssignProperty<float>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, Rgba8 newValue)
{
	CreateOrAssignProperty<Rgba8>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, Vec3 newValue)
{
	CreateOrAssignProperty<Vec3>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, Vec2 newValue)
{
	CreateOrAssignProperty<Vec2>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, IntVec2 newValue)
{
	CreateOrAssignProperty<IntVec2>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, EulerAngles newValue)
{
	CreateOrAssignProperty<EulerAngles>(keyName, newValue);
}

void NamedProperties::SetValue(std::string const& keyName, NamedProperties newValue)
{
	CreateOrAssignProperty<NamedProperties>(keyName, newValue);
}

std::string NamedProperties::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<std::string>* property = static_cast<Property<std::string>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
	}
	return defaultValue;
}

std::string NamedProperties::GetValue(std::string const& keyName, char const* defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<std::string>* property = static_cast<Property<std::string>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
	}
	return defaultValue;
}

bool NamedProperties::GetValue(std::string const& keyName, bool defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<bool>* property = static_cast<Property<bool>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string value = propertyStr->GetValue();
			if (value.compare("true"))
			{
				return true;
			}
			else if (value.compare("false"))
			{
				return false;
			}
		}
	}
	return defaultValue;
}

int NamedProperties::GetValue(std::string const& keyName, int defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<int>* property = static_cast<Property<int>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			return atoi(valueAsText.c_str());
		}
	}
	return defaultValue;
}

char NamedProperties::GetValue(std::string const& keyName, char defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<char>* property = static_cast<Property<char>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			return static_cast<char>(atoi(valueAsText.c_str()));
		}
	}
	return defaultValue;
}

float NamedProperties::GetValue(std::string const& keyName, float defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<float>* property = static_cast<Property<float>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			return static_cast<float>(atof(valueAsText.c_str()));
		}
	}
	return defaultValue;
}

Rgba8 NamedProperties::GetValue(std::string const& keyName, Rgba8 defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<Rgba8>* property = static_cast<Property<Rgba8>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			Rgba8 color;
			color.SetFromText(valueAsText.c_str());
			return color;
		}
	}
	return defaultValue;
}

Vec3 NamedProperties::GetValue(std::string const& keyName, Vec3 defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<Vec3>* property = static_cast<Property<Vec3>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			Vec3 vector;
			vector.SetFromText(valueAsText.c_str());
			return vector;
		}
	}
	return defaultValue;
}

EulerAngles NamedProperties::GetValue(std::string const& keyName, EulerAngles defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<EulerAngles>* property = static_cast<Property<EulerAngles>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			EulerAngles angles;
			angles.SetFromText(valueAsText.c_str());
			return angles;
		}
	}
	return defaultValue;
}

Vec2 NamedProperties::GetValue(std::string const& keyName, Vec2 defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<Vec2>* property = static_cast<Property<Vec2>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			Vec2 vector;
			vector.SetFromText(valueAsText.c_str());
			return vector;
		}
	}
	return defaultValue;
}

IntVec2 NamedProperties::GetValue(std::string const& keyName, IntVec2 defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<IntVec2>* property = static_cast<Property<IntVec2>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			IntVec2 vector;
			vector.SetFromText(valueAsText.c_str());
			return vector;
		}
	}
	return defaultValue;
}

NamedProperties NamedProperties::GetValue(std::string const& keyName, NamedProperties defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<NamedProperties>* property = static_cast<Property<NamedProperties>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
	}

	return defaultValue;
}

void* NamedProperties::GetValuePointer(std::string const& keyName, void* defaultValue) const
{
	HCIString lowerKeyName = keyName;
	auto entryItr = m_Properties.find(lowerKeyName);
	if (entryItr != m_Properties.end())
	{
		Property<void*>* property = static_cast<Property<void*>*>(entryItr->second);
		if (property)
		{
			return property->GetValue();
		}
		Property<std::string>* propertyStr = static_cast<Property<std::string>*>(entryItr->second);
		if (propertyStr)
		{
			std::string valueAsText = propertyStr->GetValue();
			return (void*)atoll(valueAsText.c_str());
		}
	}
	return defaultValue;
}

