#include "Engine/Core/BufferParser.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

BufferParser::BufferParser(void const* buffer, size_t size, bool flipEndian)
	: m_head(0)
	, m_size(size)
	, m_flipEndian(flipEndian)
	, m_buffer(buffer)
{
}

BufferParserResult BufferParser::ParseNext(void* out_data, size_t data_size)
{
	void* data = (void*)((size_t)m_buffer + m_head);

	if (m_head + data_size > m_size)
	{
		return BUFFER_PARSER_EOF;
	}

	if (m_flipEndian)
	{
		for (int i = 0; i < data_size; ++i)
		{
			memcpy((void*)((size_t)out_data + i), (void*)((size_t)data + data_size - i - 1), 1);
		}
	}
	else
	{
		memcpy(out_data, data, data_size);
	}

	m_head += data_size;

	return BUFFER_PARSER_SUCCESS;
}

BufferParserResult BufferParser::ParseNextByte(char& out_byte)
{
	return ParseNext(&out_byte, sizeof(char));
}

BufferParserResult BufferParser::ParseNextUnsignedByte(unsigned char& out_byte)
{
	return ParseNext(&out_byte, sizeof(unsigned char));
}

BufferParserResult BufferParser::ParseNextShort(short& out_short)
{
	return ParseNext(&out_short, sizeof(short));
}

BufferParserResult BufferParser::ParseNextUnsignedShort(unsigned short& out_short)
{
	return ParseNext(&out_short, sizeof(unsigned short));
}

BufferParserResult BufferParser::ParseNextInt32(int& out_int)
{
	return ParseNext(&out_int, sizeof(int));
}

BufferParserResult BufferParser::ParseNextUnsignedInt32(unsigned int& out_int)
{
	return ParseNext(&out_int, sizeof(unsigned int));
}

BufferParserResult BufferParser::ParseNextInt64(int64_t& out_int)
{
	return ParseNext(&out_int, sizeof(int64_t));
}

BufferParserResult BufferParser::ParseNextUnsignedInt64(uint64_t& out_int)
{
	return ParseNext(&out_int, sizeof(uint64_t));
}

BufferParserResult BufferParser::ParseNextVec2(Vec2& out_vec2)
{
	BufferParserResult result;
	result = ParseNext(&out_vec2.x, sizeof(float));
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNext(&out_vec2.y, sizeof(float));
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	return BUFFER_PARSER_SUCCESS;
}

BufferParserResult BufferParser::ParseNextVec3(Vec3& out_vec3)
{
	BufferParserResult result;
	result = ParseNext(&out_vec3.x, sizeof(float));
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNext(&out_vec3.y, sizeof(float));
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNext(&out_vec3.z, sizeof(float));
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	return BUFFER_PARSER_SUCCESS;
}

BufferParserResult BufferParser::ParseNextAABB2(AABB2& out_aabb)
{
	BufferParserResult result;
	result = ParseNextVec2(out_aabb.m_min);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNextVec2(out_aabb.m_max);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	return BUFFER_PARSER_SUCCESS;
}

BufferParserResult BufferParser::ParseNextRgba8(Rgba8& out_color)
{
	BufferParserResult result;
	result = ParseNextUnsignedByte(out_color.r);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNextUnsignedByte(out_color.g);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNextUnsignedByte(out_color.b);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	result = ParseNextUnsignedByte(out_color.a);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}
	return BUFFER_PARSER_SUCCESS;
}

BufferParserResult BufferParser::ParseNextFloat(float& out_float)
{
	return ParseNext(&out_float, sizeof(float));
}

BufferParserResult BufferParser::ParseNextDouble(double& out_double)
{
	return ParseNext(&out_double, sizeof(double));
}

BufferParserResult BufferParser::ParseNextString(std::string& out_string)
{
	int length;

	BufferParserResult result = ParseNextInt32(length);
	if (result != BUFFER_PARSER_SUCCESS)
	{
		return result;
	}

	out_string.resize(length);
	result = ParseNext(out_string.data(), length);

	return result;
}

BufferParserResult BufferParser::ParseNextRawString(std::string& out_string)
{
	while (m_head < m_size)
	{
		char c;
		BufferParserResult result = ParseNextByte(c);
		if (result != BUFFER_PARSER_SUCCESS)
		{
			return result;
		}
		if (c == '\0')
		{
			break;
		}
		out_string.push_back(c);
	}

	return BUFFER_PARSER_SUCCESS;
}

