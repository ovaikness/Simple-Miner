#pragma once


//-----------------------------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include "Engine/Math/Mat44.hpp"
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs

class AudioSystemConfig
{

};

/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem(AudioSystemConfig const& config = {});
	virtual ~AudioSystem();

public:
	void						Startup();
	void						Shutdown();
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual void				SetNumListeners(int numListeners);
	virtual void				UpdateListeners(int listenerIndex, Vec3 const& listenerPosition, Vec3 const& listenerForward, Vec3 const& listenerUp);
	virtual SoundPlaybackID 	StartSoundAt(SoundID soundId, Vec3 const& soundPosition, bool isLooped = false, float volume = 1.f, float balance = 1.f, float speed = 1.f, bool isPaused = false, float pitch = 1.f);
	virtual void				SetSoundPosition(SoundPlaybackID soundPlaybackID, Vec3 const& soundPosition);
	bool						IsPlaying(SoundPlaybackID soundPlaybackID);
	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath , FMOD_MODE mode = FMOD_DEFAULT);
	virtual SoundPlaybackID		StartSound( SoundID soundID, bool isLooped=false, float volume=1.f, float pitch=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)
	virtual void				SetSoundPlaybackPitch( SoundPlaybackID soundPlaybackID, float pitch);		// pitch
	virtual void				ValidateResult( FMOD_RESULT result );

protected:
	Mat44								m_renderMatrix =
	{
		Vec3(0.f,-1.f,0.f),
		Vec3(0.f,0.f,1.f),
		Vec3(-1.f,0.f,0.f),
		Vec3(0.f,0.f,0.f)
	};
	AudioSystemConfig					m_config;
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};

