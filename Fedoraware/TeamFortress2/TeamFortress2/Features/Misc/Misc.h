#pragma once

#include "../../SDK/SDK.h"

static float angleDiffRad(float a1, float a2) noexcept
{
	float delta = Utils::NormalizeRad(a1 - a2);
	if (a1 > a2)
	{
		if (delta >= PI) { delta -= PI * 2; }
	}
	else
	{
		if (delta <= -PI) { delta += PI * 2; }
	}
	return delta;
}

class CMisc
{
private:
	void AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoiseMakerSpam(CBaseEntity* pLocal);
	void PingReducer();
	void ExtendFreeze(CBaseEntity* pLocal);
	void Freecam(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoJoin();
	void InitSpamKV(void* pKV);
	void ChatSpam();
	bool steamCleared = false;

public:
	void Run(CUserCmd* pCmd);
	void RunLate(CUserCmd* pCmd);
	void CheatsBypass();
	void ServerHitbox();
	void AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd);
	void WeaponSway();
	void DetectChoke();
	void LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal);
	void EdgeJump(CUserCmd* pCmd, int nOldFlags);
	void BypassPure();
	void AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoPeek(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoPush();
	void SteamRPC();
	void UnlockAchievements();
	void LockAchievements();

	std::vector<std::string> strings;
	Vec3 PeekReturnPos;
};

inline CMisc g_Misc;

#define MAX_NOTIFY_SIZE 8

class NotifyText
{
public:
	std::string m_text;
	Color_t m_color;
	float m_time;

public:
	NotifyText()
	{
	}

	NotifyText(const std::string& text, Color_t Color, float time) : m_text{text}, m_color{Color}, m_time{time}
	{
	}
};

class CNotifications
{
private:
	std::vector<std::shared_ptr<NotifyText>> m_vNotificationTexts;

public:
	CNotifications() : m_vNotificationTexts{}
	{
	}


	__forceinline void Add(const std::string& text, Color_t color = {255, 255, 255, 255},
	                       float time = Vars::Visuals::despawnTime.m_Var)
	{
		m_vNotificationTexts.push_back(std::make_shared<NotifyText>(text, color, time));
	}

	void Think();
};

inline CNotifications g_Notifications;
