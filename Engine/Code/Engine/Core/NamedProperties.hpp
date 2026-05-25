#pragma once

#include "Engine/Core/HCIString.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <map>

class PropertyBase
{
public:
	virtual ~PropertyBase()
	{
	}

	virtual PropertyBase* Clone() const
	{
		return nullptr;
	}
};

template <typename T>
class Property : public PropertyBase
{
public:
	T m_Value;
public:
	Property() = default;
	Property(T value) : m_Value(value)
	{
	}

	T const& GetValue() const
	{
		return m_Value;
	}

	T& GetValue()
	{
		return m_Value;
	}

	void SetValue(T value)
	{
		m_Value = value;
	}

	PropertyBase* Clone() const override
	{
		return new Property<T>(m_Value);
	}
};

class NamedProperties
{
public:
	std::map<HCIString, PropertyBase*> m_Properties;
	NamedProperties();
	NamedProperties(NamedProperties const& copy);
	NamedProperties(NamedProperties&& move);
	NamedProperties& operator=(NamedProperties&& move);
	NamedProperties& operator=(NamedProperties const& copy);
	~NamedProperties();
public:
	void PopulateFromXmlElementAttributes(XmlElement const& element);

	void		SetValue(std::string const& keyName, std::string const& newValue);
	void        SetValue(std::string const& keyName, char newValue);
	void		SetValue(std::string const& keyName, void* newValue);
	void		SetValue(std::string const& keyName, bool newValue);
	void		SetValue(std::string const& keyName, int newValue);
	void		SetValue(std::string const& keyName, float newValue);
	void		SetValue(std::string const& keyName, Rgba8 newValue);
	void		SetValue(std::string const& keyName, Vec3 newValue);
	void		SetValue(std::string const& keyName, Vec2 newValue);
	void		SetValue(std::string const& keyName, IntVec2 newValue);
	void		SetValue(std::string const& keyName, EulerAngles newValue);
	void 		SetValue(std::string const& keyName, NamedProperties newValue);

	std::string GetValue(std::string const& keyName, std::string const& defaultValue) const;
	std::string GetValue(std::string const& keyName, char const* defaultValue) const;
	void*		GetValuePointer(std::string const& keyName, void* defaultValue) const;
	bool        GetValue(std::string const& keyName, bool defaultValue) const;
	int			GetValue(std::string const& keyName, int defaultValue) const;
	char		GetValue(std::string const& keyName, char defaultValue) const;
	float		GetValue(std::string const& keyName, float defaultValue) const;
	Rgba8		GetValue(std::string const& keyName, Rgba8 defaultValue) const;
	Vec3		GetValue(std::string const& keyName, Vec3 defaultValue) const;
	EulerAngles GetValue(std::string const& keyName, EulerAngles defaultValue) const;
	Vec2        GetValue(std::string const& keyName, Vec2 defaultValue) const;
	IntVec2		GetValue(std::string const& keyName, IntVec2 defaultValue) const;
	NamedProperties GetValue(std::string const& keyName, NamedProperties defaultValue) const;

	template <typename T>
	void CreateOrAssignProperty(std::string const& keyName, T value)
	{
		HCIString lowerKeyName = keyName;
		auto entryItr = m_Properties.find(lowerKeyName);
		if (entryItr != m_Properties.end())
		{
			PropertyBase* entry = entryItr->second;
			Property<T>* prop = dynamic_cast<Property<T>*>(entry);

			if (entry == nullptr)
			{
				delete entry;
				m_Properties.erase(entryItr);
			}
			else if (prop == nullptr)
			{
				delete entry;
				m_Properties.erase(entryItr);
				prop = new Property<T>(value);
				m_Properties[lowerKeyName] = (PropertyBase*)prop;
				return;
			}
			else
			{
				prop->SetValue(value);
				return;
			}
		}
		
		Property<T>* property = new Property<T>(value);
		m_Properties[lowerKeyName] = (PropertyBase*)property;
	}
};