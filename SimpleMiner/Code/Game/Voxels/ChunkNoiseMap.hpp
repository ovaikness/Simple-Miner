#pragma once

template<typename T>
class ChunkNoiseMap
{
public:
	ChunkNoiseMap(int width, int height, int padding)
		: m_width(width)
		, m_height(height)
		, m_padding(padding)
	{
		m_values = new T[(m_width + (m_padding * 2)) * (m_height + (m_padding * 2))];
	}
	~ChunkNoiseMap()
	{
		delete m_values;
	}

public:
	void IsInRange(int xPos, int yPos)
	{
		int paddedWidth = m_width + m_padding * 2;
		int paddedHeight = m_height + m_padding * 2;

		int yPosPadded = yPos + m_padding;
		int xPosPadded = xPos + m_padding;

		return xPosPadded >= 0 && xPosPadded < paddedWidth && yPosPadded >= 0 && yPosPadded < paddedHeight;
	}
	
	void Set(int xPos, int yPos, T value)
	{
		int paddedWidth = m_width + m_padding * 2;
		int paddedHeight = m_height + m_padding * 2;

		int yPosPadded = yPos + m_padding;
		int xPosPadded = xPos + m_padding;

		m_values[yPosPadded * paddedWidth + xPosPadded] = value;
	}

	T Get(int xPos, int yPos)
	{
		int paddedWidth = m_width + m_padding * 2;
		int paddedHeight = m_height + m_padding * 2;

		int yPosPadded = yPos + m_padding;
		int xPosPadded = xPos + m_padding;

		return m_values[yPosPadded * paddedWidth + xPosPadded];
	}
protected:
	T* m_values;
	int m_width;
	int m_height;
	int m_padding;
};