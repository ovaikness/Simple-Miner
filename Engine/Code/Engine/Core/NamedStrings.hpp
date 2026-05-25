#pragma once
#include <map>
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/TinyXMLUtils.hpp"

class NamedStrings
{
public:
	class NamedStrings() = default;
	void PopulateFromXmlElementAttributes(XmlElement const& element);

	void		SetValue(std::string const& keyName, std::string const& newValue);
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
protected:
	std::map<std::string, std::string> m_keyValuePairs;
};