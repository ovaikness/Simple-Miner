#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <map>

//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( int maxLength, char const* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

Strings SplitStringOnDelimiter(std::string const& originalString, char delimiterToSplitOn)
{
	Strings strings;
	size_t firstIndex = 0;
	for (size_t endIndex = 0; endIndex < originalString.size(); ++endIndex)
	{
		if (originalString.at(endIndex) == delimiterToSplitOn)
		{
			strings.push_back(originalString.substr(firstIndex, endIndex - firstIndex));
			firstIndex = endIndex + 1;
		}
	}
	strings.push_back(originalString.substr(firstIndex, originalString.size() - firstIndex));
	return strings;
}

Strings SplitStringOnDelimiterWithQuotes(std::string const& originalString, char delimiterToSplitOn)
{
	Strings strings;
	size_t firstIndex = 0;
	bool insideQuotes = false;

	for (size_t endIndex = 0; endIndex < originalString.size(); ++endIndex)
	{
		if (originalString.at(endIndex) == '\"')
		{
			insideQuotes = !insideQuotes; // Toggle the insideQuotes flag
		}
		else if (!insideQuotes && originalString.at(endIndex) == delimiterToSplitOn)
		{
			// Split only if we're not inside quotes
			strings.push_back(originalString.substr(firstIndex, endIndex - firstIndex));
			firstIndex = endIndex + 1;
		}
	}

	// Add the remaining string after the last delimiter
	if (firstIndex < originalString.size())
	{
		strings.push_back(originalString.substr(firstIndex));
	}

	// Remove the quotes after splitting
	for (auto& str : strings)
	{
		if (str.size() > 0 && str.front() == '\"' && str.back() == '\"')
		{
			str = str.substr(1, str.size() - 2); // Remove leading and trailing quotes
		}
	}


	return strings;
}

std::string RemoveSubstr(std::string str, std::string const& toRemove)
{
	size_t index = str.find(toRemove);
	if (index != std::string::npos)
	{
		return str.substr(0, index) + str.substr(index + toRemove.size(), str.size() - index - toRemove.size());
	}
	return str;
}

std::string RemoveAllSubstr(std::string str, std::string const& toRemove)
{
	std::string result = str;
	size_t index;

	if (toRemove.empty())
	{
		return result;
	}

	while ((index = result.find(toRemove)) != std::string::npos)
	{
		result.erase(index, toRemove.length());
	}

	return result;
}





