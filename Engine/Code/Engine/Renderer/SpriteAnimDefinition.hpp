#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

enum class SpriteAnimPlaybackType
{
	ONCE,		// for 5-frame anim, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,		// for 5-frame anim, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG,	// for 5-frame anim, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
	SINGLE,     // single image
};

//------------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition() = default;
	SpriteAnimDefinition(int startSpriteIndex, int endSpriteIndex,
		float durationSeconds, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);
	SpriteAnimDefinition(const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex,
		float durationSeconds, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);

	bool Finished(float durationSeconds) const;
	void SetSpriteSheet(SpriteSheet const& sheet);
	const SpriteDefinition GetSpriteDefAtTime(float seconds) const;
	SpriteAnimPlaybackType GetPlaybackMode() const;
	Texture const& GetTexture() const;
	float GetDurationSeconds() const;
private:
	SpriteSheet        m_spriteSheet;
	int				   m_startSpriteIndex = -1;
	int				   m_endSpriteIndex = -1;
	float			   m_durationSeconds = 1.f;
	SpriteAnimPlaybackType	m_playbackType = SpriteAnimPlaybackType::LOOP;
};
