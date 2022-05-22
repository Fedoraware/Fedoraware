#pragma once

#include "../../SDK/SDK.h"

class CMisc {
	void AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoiseMakerSpam(CBaseEntity* pLocal);
	void PingReducer();
	void ExtendFreeze(CBaseEntity* pLocal);
	void Teleport(const CUserCmd* pCmd);
	void Freecam(CUserCmd* pCmd, CBaseEntity* pLocal);
	void ChatSpam();

	bool SteamCleared = false;

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
	void AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoPeek(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoPush();
	void SteamRPC();
	void UnlockAchievements();
	void LockAchievements();

	Vec3 PeekReturnPos;
};

inline CMisc g_Misc;

constexpr auto MAX_NOTIFY_SIZE = 8;

class NotifyText {
public:
	std::string Text;
	Color_t Color;
	float Time;

	NotifyText() { }
	NotifyText(std::string text, Color_t color, float time) : Text{std::move(text)}, Color{color}, Time{time} { }
};

class CNotifications {
	std::vector<std::shared_ptr<NotifyText>> NotificationTexts;

public:
	CNotifications() = default;

	__forceinline void Add(const std::string& text, Color_t color = {255, 255, 255, 255},
	                       float time = Vars::Visuals::despawnTime.m_Var)
	{
		NotificationTexts.push_back(std::make_shared<NotifyText>(text, color, time));
	}

	void Think();
};

inline CNotifications g_Notifications;
