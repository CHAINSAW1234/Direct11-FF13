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
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;
	_bool		m_bPause = false;

private:
	void LoadSoundFile();

public:
	static CSound_Manager* Create();
	virtual void Free() override;

};

END
