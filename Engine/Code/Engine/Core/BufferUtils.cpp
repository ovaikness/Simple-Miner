#include "Engine/Core/BufferUtils.hpp"
#include <algorithm>

void ReverseByteOrderInPlace(void* bytes, size_t size)
{
	unsigned char* data = (unsigned char*)bytes;
	for (size_t i = 0; i < size / 2; ++i)
	{
		std::swap(data[i], data[size - i - 1]);
	}
}