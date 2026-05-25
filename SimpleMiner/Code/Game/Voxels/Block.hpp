#pragma once
#include <cinttypes>

enum BlockStateBitFlags
{
	BLOCK_BIT_IS_SKY = 1 << 0,
	BLOCK_BIT_LIGHT_DIRTY = 1 << 1,
	BLOCK_BIT_FULL_OPAQUE = 1 << 2,
	BLOCK_BIT_IS_SOLID = 1 << 3,
	BLOCK_BIT_IS_VISIBLE = 1 << 4,
};

constexpr uint8_t SKY_LIGHT_MASK = 0xF0;
constexpr uint8_t BLOCK_LIGHT_MASK = 0x0F;
constexpr uint8_t SKY_LIGHT_BITSHIFT = 4;
constexpr uint8_t BLOCK_LIGHT_BITSHIFT = 0;

class BlockDef;

class Block
{
public:
	uint8_t m_type = 0;
	uint8_t m_light = 0;
	uint8_t m_state = 0;
public:
	~Block() = default;
	Block() = default;

	uint8_t GetSkyLight() const;
	uint8_t GetBlockLight() const;

	void SetBlockLight(uint8_t light);
	void SetSkyLight(uint8_t light);

	bool GetLightingDirty() const;
	bool GetIsSky() const;

	void SetIsSky(bool value);
	void SetLightingDirty(bool value);

	BlockDef GetBlockDef();
};