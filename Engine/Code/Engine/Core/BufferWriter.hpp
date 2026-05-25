#pragma once

#include <vector>

struct Rgba8;
struct Vec2;
struct Vec3;
struct AABB2;

class BufferWriter
{
public:
	bool m_flipEndian;
	size_t m_head;
	std::vector<uint8_t>& m_buffer;
public:
	BufferWriter(std::vector<uint8_t>& buffer, bool flipEndian = false);
	~BufferWriter();

	void WriteData(void* data, size_t data_size);
	void WriteDataAtOffset(void* data, size_t data_size, size_t offset);

	void WriteIntAtOffset(unsigned int i, size_t offset);

	void WriteNextVec2(const Vec2& v);
	void WriteNextVec3(const Vec3& v);
	void WriteNextAABB2(const AABB2& aabb);
	void WriteNextRgba8(const Rgba8& color);

	void WriteNextByte(char byte);
	void WriteNextUnsignedByte(unsigned char byte);
	void WriteNextShort(short s);
	void WriteNextUnsignedShort(unsigned short s);
	void WriteNextInt32(int i);
	void WriteNextUnsignedInt32(unsigned int i);
	void WriteNextInt64(int64_t i);
	void WriteNextUnsignedInt64(uint64_t i);
	void WriteNextFloat(float f);
	void WriteNextDouble(double d);
	void WriteNextString(const char* str);
	void WriteNextRawString(const char* str);
};