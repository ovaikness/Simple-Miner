#include "Game/Voxels/Block.hpp"
#include "Game/Voxels/BlockDef.hpp"

uint8_t Block::GetSkyLight() const
{
	return (m_light & SKY_LIGHT_MASK) >> SKY_LIGHT_BITSHIFT;
}

uint8_t Block::GetBlockLight() const
{
	return (m_light & BLOCK_LIGHT_MASK) >> BLOCK_LIGHT_BITSHIFT;
}

void Block::SetBlockLight(uint8_t light)
{
	m_light &= ~BLOCK_LIGHT_MASK;
	m_light |= light << BLOCK_LIGHT_BITSHIFT;
}

void Block::SetSkyLight(uint8_t light)
{
	m_light &= ~SKY_LIGHT_MASK;
	m_light |= light << SKY_LIGHT_BITSHIFT;
}

bool Block::GetLightingDirty() const
{
	return (m_state & BLOCK_BIT_LIGHT_DIRTY);
}

bool Block::GetIsSky() const
{
	return (m_state & BLOCK_BIT_IS_SKY);
}

void Block::SetIsSky(bool value)
{
	if (value)
	{
		m_state |= BLOCK_BIT_IS_SKY;
	}
	else
	{
		m_state &= ~(BLOCK_BIT_IS_SKY);
	}
}

void Block::SetLightingDirty(bool value)
{
	if (value)
	{
		m_state |= BLOCK_BIT_LIGHT_DIRTY;
	}
	else
	{
		m_state &= ~(BLOCK_BIT_LIGHT_DIRTY);
	}
}

BlockDef Block::GetBlockDef()
{
	BlockDef def = BlockDef::s_blockDefRegistry->GetBlockDefByType(m_type);
	return def;
}

