#include "SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <cmath>
SpriteAnimDefinition::SpriteAnimDefinition(int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType)
	: m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_durationSeconds(durationSeconds)
	, m_playbackType(playbackType)
{
}
SpriteAnimDefinition::SpriteAnimDefinition(const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/)
	: m_spriteSheet(sheet)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_durationSeconds(durationSeconds)
	, m_playbackType(playbackType)
{

}

SpriteAnimPlaybackType SpriteAnimDefinition::GetPlaybackMode() const
{
	return m_playbackType;
}

Texture const& SpriteAnimDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

float SpriteAnimDefinition::GetDurationSeconds() const
{
	return m_durationSeconds;
}

bool SpriteAnimDefinition::Finished(float seconds) const
{
	return seconds >= m_durationSeconds;
}
void SpriteAnimDefinition::SetSpriteSheet(SpriteSheet const& sheet)
{
	m_spriteSheet = sheet;
}

const SpriteDefinition SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{

	if (m_durationSeconds <= 0.0f)
	{
		return m_spriteSheet.GetSpriteDefinition(m_startSpriteIndex);
	}
	if (m_startSpriteIndex == m_endSpriteIndex)
	{
		return m_spriteSheet.GetSpriteDefinition(m_startSpriteIndex);
	}
	switch (m_playbackType)
	{
		case SpriteAnimPlaybackType::LOOP:
		{
			float secondsOffset = fmod(seconds, m_durationSeconds);
			float decimalIndex = RangeMap(secondsOffset, 0.f, m_durationSeconds, static_cast<float>(m_startSpriteIndex), static_cast<float>(m_endSpriteIndex + 1));
			int index = static_cast<int>(floorf(decimalIndex));
			if (index > m_endSpriteIndex)
			{
				index = m_endSpriteIndex;
			}
			return m_spriteSheet.GetSpriteDefinition(index);
		}
		case SpriteAnimPlaybackType::PINGPONG:
		{
			float secondsOffset = fmod(seconds, m_durationSeconds);
			bool flip = static_cast<int>(floorf(seconds / m_durationSeconds)) % 2 == 1;
			float decimalIndex = flip ? RangeMap(secondsOffset, 0, m_durationSeconds, static_cast<float>(m_startSpriteIndex), static_cast<float>(m_endSpriteIndex + 1))
				: RangeMap(secondsOffset, 0, m_durationSeconds, static_cast<float>(m_endSpriteIndex + 1), static_cast<float>(m_startSpriteIndex));
			int index = static_cast<int>(floorf(decimalIndex));
			if (index > m_endSpriteIndex)
			{
				index = m_endSpriteIndex;
			}
			return m_spriteSheet.GetSpriteDefinition(index);
		}
		case SpriteAnimPlaybackType::ONCE:
		{
			float index = RangeMapClamped(seconds, 0.f, m_durationSeconds, static_cast<float>(m_startSpriteIndex), static_cast<float>(m_endSpriteIndex + 1));
			if (index > m_endSpriteIndex)
			{
				index = static_cast<float>(m_endSpriteIndex);
			}
			int roundedIndex = static_cast<int>(floorf(index));
			return m_spriteSheet.GetSpriteDefinition(roundedIndex);
		}
		case SpriteAnimPlaybackType::SINGLE:
		{
			return m_spriteSheet.GetSpriteDefinition(m_startSpriteIndex);
		}
		default:
		{
			ERROR_AND_DIE("Unrecognized sprite animation playback type!");
		}
	}
}