#pragma once

#include "../../Includes/Includes.h"

class CBaseHudChat
{
public:
	void ChatPrintf(int pIndex, const char *fmt, ...)
	{
		typedef void(_cdecl* FN)(void*, int, int, const char*, ...);
		GetVFunc<FN>(this, 19)(this, pIndex, 0, fmt);
	}

	void StartMessageMode(int iMessageModeType)
	{
		typedef void(__thiscall* FN)(void*, int);
		GetVFunc<FN>(this, 20)(this, iMessageModeType);
	}
};

class CClientModeShared
{
public:
	char szPad[28];
	CBaseHudChat *m_pChatElement;

	bool IsChatPanelOutOfFocus()
	{
		void* CHudChat = GetVFunc<void*(__thiscall *)(void*)>(this, 19)(this);
		return (CHudChat && !*(float *)((DWORD)CHudChat + 0xFC));
	}
};

class ClientModeTFNormal
{
public:
};

struct CSoundParameters
{
	CSoundParameters()
	{
		channel = 0; // 0
		volume = 1.0f;  // 1.0f
		pitch = 100; // 100

		pitchlow = 100;
		pitchhigh = 100;

		soundlevel = SNDLVL_NORM; // 75dB
		soundname[0] = 0;
		play_to_owner_only = false;
		count = 0;

		delay_msec = 0;
	}

	int				channel;
	float			volume;
	int				pitch;
	int				pitchlow, pitchhigh;
	soundlevel_t	soundlevel;
	// For weapon sounds...
	bool			play_to_owner_only;
	int				count;
	char 			soundname[128];
	int				delay_msec;
};

struct EmitSound_t
{
	EmitSound_t() :
		m_nChannel(0),
		m_pSoundName(0),
		m_flVolume(1.0f),
		m_SoundLevel(0),
		m_nFlags(0),
		m_nPitch(100),
		m_nSpecialDSP(0),
		m_pOrigin(0),
		m_flSoundTime(0.0f),
		m_pflSoundDuration(0),
		m_bEmitCloseCaption(true),
		m_bWarnOnMissingCloseCaption(false),
		m_bWarnOnDirectWaveReference(false),
		m_nSpeakerEntity(-1),
		m_UtlVecSoundOrigin(),
		m_hSoundScriptHandle(-1)
	{
	}

	EmitSound_t(const CSoundParameters& src);

	int							m_nChannel;
	char const* m_pSoundName;
	float						m_flVolume;
	int				m_SoundLevel;
	int							m_nFlags;
	int							m_nPitch;
	int							m_nSpecialDSP;
	const Vector* m_pOrigin;
	float						m_flSoundTime; ///< NOT DURATION, but rather, some absolute time in the future until which this sound should be delayed
	float* m_pflSoundDuration;
	bool						m_bEmitCloseCaption;
	bool						m_bWarnOnMissingCloseCaption;
	bool						m_bWarnOnDirectWaveReference;
	int							m_nSpeakerEntity;
	mutable CUtlVector< Vector >	m_UtlVecSoundOrigin;  ///< Actual sound origin(s) (can be multiple if sound routed through speaker entity(ies) )
	mutable short		m_hSoundScriptHandle;
};