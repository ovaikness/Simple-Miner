#include "Engine/Core/BufferWriter.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

BufferWriter::BufferWriter(std::vector<uint8_t>& buffer, bool flipEndian /*= false*/)
	: m_flipEndian(flipEndian)
	, m_head(0)
	, m_buffer(buffer)
{
}

BufferWriter::~BufferWriter()
{

}

void BufferWriter::WriteData(void* data, size_t data_size)
{
	if (m_head + data_size > m_buffer.size())
	{
		m_buffer.resize(m_head + data_size);
	}
	memcpy(m_buffer.data() + m_head, data, data_size);
	if (m_flipEndian)
	{
		for (size_t i = 0; i < data_size / 2; ++i)
		{
			std::swap(m_buffer[m_head + i], m_buffer[m_head + data_size - i - 1]);
		}
	}
	m_head += data_size;
}

void BufferWriter::WriteDataAtOffset(void* data, size_t data_size, size_t offset)
{
	if (offset + data_size > m_buffer.size())
	{
		m_buffer.resize(offset + data_size);
	}
	if (m_flipEndian)
	{
		for (size_t i = 0; i < data_size / 2; ++i)
		{
			std::swap(((uint8_t*)data)[i], ((uint8_t*)data)[data_size - i - 1]);
		}
	}

	memcpy(m_buffer.data() + offset, data, data_size);
}

void BufferWriter::WriteIntAtOffset(unsigned int i, size_t offset)
{
	WriteDataAtOffset(&i, sizeof(unsigned int), offset);
}

void BufferWriter::WriteNextVec2(Vec2 const& v)
{
	WriteNextFloat(v.x);
	WriteNextFloat(v.y);
}

void BufferWriter::WriteNextVec3(Vec3 const& v)
{
	WriteNextFloat(v.x);
	WriteNextFloat(v.y);
	WriteNextFloat(v.z);
}

void BufferWriter::WriteNextAABB2(AABB2 const& aabb)
{
	WriteNextVec2(aabb.m_min);
	WriteNextVec2(aabb.m_max);
}

void BufferWriter::WriteNextRgba8(const Rgba8& color)
{
	WriteNextUnsignedByte(color.r);
	WriteNextUnsignedByte(color.g);
	WriteNextUnsignedByte(color.b);
	WriteNextUnsignedByte(color.a);
}

void BufferWriter::WriteNextByte(char byte)
{
	WriteData(&byte, sizeof(unsigned char));
}

void BufferWriter::WriteNextUnsignedByte(unsigned char byte)
{
	WriteData(&byte, sizeof(unsigned char));
}

void BufferWriter::WriteNextShort(short s)
{
	WriteData(&s, sizeof(unsigned short));
}

void BufferWriter::WriteNextUnsignedShort(unsigned short s)
{
	WriteData(&s, sizeof(unsigned short));
}

void BufferWriter::WriteNextInt32(int i)
{
	WriteData(&i, sizeof(int));
}

void BufferWriter::WriteNextUnsignedInt32(unsigned int i)
{
	WriteData(&i, sizeof(unsigned int));
}

void BufferWriter::WriteNextInt64(int64_t i)
{
	WriteData(&i, sizeof(int64_t));
}

void BufferWriter::WriteNextUnsignedInt64(uint64_t i)
{
	WriteData(&i, sizeof(uint64_t));
}

void BufferWriter::WriteNextFloat(float f)
{
	WriteData(&f, sizeof(float));
}

void BufferWriter::WriteNextDouble(double d)
{
	WriteData(&d, sizeof(double));
}

void BufferWriter::WriteNextString(const char* str)
{
	size_t len = strlen(str);
	WriteData(&len, sizeof(size_t));
	WriteData((void*)str, len);
}

void BufferWriter::WriteNextRawString(const char* str)
{
	char c;
	while ((c = *str++) != '\0')
	{
		WriteNextByte(c);
	}
}

