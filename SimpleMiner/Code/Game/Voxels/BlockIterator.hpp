#pragma once

#include "Engine/Math/Vec3.hpp"

class Block;
class Chunk;

class BlockIterator
{
public:
	Chunk* m_chunk = nullptr;
	int m_index = 0;
public:
	BlockIterator(Chunk* chunk, int index);
	/*
	* @brief Returns the world coordinates of the block offset from the corner of the bottom left of the block.
	* (For example, it will not return 3,3,0 but 3.5,3.5,0.5)
	*/
	Vec3 GetWorldCenter() const;

	Block* GetBlock() const;

	BlockIterator GetEastNeighbor() const;
	BlockIterator GetWestNeighbor() const;
	BlockIterator GetSkywardNeighbor() const;
	BlockIterator GetGroundwardNeighbor() const;
	BlockIterator GetNorthNeighbor() const;
	BlockIterator GetSouthNeighbor() const;
protected:
	BlockIterator();
};