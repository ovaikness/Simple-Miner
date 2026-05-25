#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"

typedef tinyxml2::XMLDocument  XmlDocument;
typedef tinyxml2::XMLElement   XmlElement;
typedef tinyxml2::XMLAttribute XmlAttribute;
typedef tinyxml2::XMLError	   XmlResult;

int			ParseXMLAttribute(XmlElement const& element, char const* attributeName, int defaultValue);
bool		ParseXMLAttribute(XmlElement const& element, char const* attributeName, bool defaultValue);
wchar_t     ParseXMLAttribute(XmlElement const& element, char const* attributeName, wchar_t defaultValue);
char		ParseXMLAttribute(XmlElement const& element, char const* attributeName, char defaultValue);
float		ParseXMLAttribute(XmlElement const& element, char const* attributeName, float defaultValue);
FloatRange  ParseXMLAttribute(XmlElement const& element, char const* attributeName, FloatRange defaultValue);
Rgba8		ParseXMLAttribute(XmlElement const& element, char const* attributeName, Rgba8 defaultValue);
Vec3		ParseXMLAttribute(XmlElement const& element, char const* attributeName, Vec3 defaultValue);
EulerAngles ParseXMLAttribute(XmlElement const& element, char const* attributeName, EulerAngles defaultValue);
Vec2		ParseXMLAttribute(XmlElement const& element, char const* attributeName, Vec2 defaultValue);
IntVec2		ParseXMLAttribute(XmlElement const& element, char const* attributeName, IntVec2 defaultValue);
std::string ParseXMLAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue);
std::string ParseXMLAttribute(XmlElement const& element, char const* attributeName, std::string defaultValue);
Strings		ParseXMLAttribute(XmlElement const& element, char const* attributeName, Strings defaultValue);

void 	    WriteXMLAttribute(XmlElement* element, char const* attributeName, int value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, bool value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, wchar_t value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, char value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, float value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, FloatRange value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, Rgba8 value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, Vec3 value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, EulerAngles value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, Vec2 value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, IntVec2 value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, char const* value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, std::string value);
void		WriteXMLAttribute(XmlElement* element, char const* attributeName, Strings value);