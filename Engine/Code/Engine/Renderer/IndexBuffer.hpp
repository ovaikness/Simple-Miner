#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct ID3D11Buffer;

class IndexBuffer
{
	friend class Renderer;
public:
	IndexBuffer(unsigned int size);
	IndexBuffer(IndexBuffer const& copy) = delete;
	virtual ~IndexBuffer();

	unsigned int GetSize() const;
protected:
	ID3D11Buffer* m_buffer = nullptr;
	unsigned int m_size = 0;
};