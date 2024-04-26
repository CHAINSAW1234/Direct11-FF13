#pragma once

#include "Base.h"
#include "FMOD/fmod.h"
#include "Engine_Defines.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.4f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	enum CHANNELID { BGM, BGM_FIELD, PLAYER, FOLLOWER, EFFECT1, EFFECT2, EFFECT3, UI1, UI2, UI3, UI4, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	_uint	VolumeUp(CHANNELID eID, _float _vol);
	_uint	VolumeDown(CHANNELID eID, _float _vol);
	_uint	BGMVolumeUp(_float _vol);
	_uint	BGMVolumeDown(_float _vol);
	_uint	Pause(CHANNELID eID);
	void	PlaySound(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	void	PlaySoundOnce(TCHAR* pSoundKey, CHANNELID eID, _float _vol);

	void	SetChannelOneOff(_bool bOneOff, CHANNELID eID) { m_bChannelOneOff[eID] = bOneOff; }
	void	PlaySoundOneOff(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	int		PlaySoundLoop(TCHAR* pSoundKey, CHANNELID eID, _float _vol);

	void	SetLoopCount(CHANNELID eID, _uint iCount) { FMOD_Channel_SetLoopCount(m_pChannelArr[eID], iCount); }
	void	PlayBGM(TCHAR* pSoundKey);
	void	StopSound(CHANNELID eID);
	void	StopAll();

private:
	_bool	m_bChannelOneOff[MAXCHANNEL] = { false };

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT-0.2f;
	FMOD_BOOL m_bool;

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
	_bool		m_bPause = false;

private:
	void LoadSoundFile();

public:
	static CSound_Manager* Create();
	virtual void Free() override;

};

END
