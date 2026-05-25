#pragma once

#include <string>
#include <cinttypes>

struct Rgba8;
struct Vec2;
struct Vec3;
struct AABB2;


enum EndianMode
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
	NATIVE_ENDIAN
};

enum BufferParserResult
{
	BUFFER_PARSER_SUCCESS = 0,
	BUFFER_PARSER_ERROR,
	BUFFER_PARSER_EOF
};


class BufferParser
{
public:
	bool m_flipEndian;

	size_t m_head;
	size_t m_size;
	void const* m_buffer;
public:
	BufferParser(void const* buffer, size_t size, bool flipEndian = false);

	void SetHead(size_t head) 
	{
		m_head = head;
	}
	BufferParserResult ParseNext(void* out_data, size_t data_size);

	BufferParserResult ParseNextByte(char& out_byte);
	BufferParserResult ParseNextUnsignedByte(unsigned char& out_byte);
	BufferParserResult ParseNextShort(short& out_short);
	BufferParserResult ParseNextUnsignedShort(unsigned short& out_short);
	BufferParserResult ParseNextInt32(int& out_int);
	BufferParserResult ParseNextUnsignedInt32(unsigned int& out_int);
	BufferParserResult ParseNextInt64(int64_t& out_int);
	BufferParserResult ParseNextUnsignedInt64(uint64_t& out_int);

	BufferParserResult ParseNextVec2(Vec2& out_vec2);
	BufferParserResult ParseNextVec3(Vec3& out_vec3);
	BufferParserResult ParseNextAABB2(AABB2& out_aabb);
	BufferParserResult ParseNextRgba8(Rgba8& out_color);

	BufferParserResult ParseNextFloat(float& out_float);
	BufferParserResult ParseNextDouble(double& out_double);

	BufferParserResult ParseNextString(std::string& out_string);
	BufferParserResult ParseNextRawString(std::string& out_string);
};