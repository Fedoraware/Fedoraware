#pragma once
#include "../../SDK/SDK.h"

class CMisc {
	void AccurateMovement(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoStrafe(CUserCmd* pCmd, CBaseEntity* pLocal);
	void NoiseMakerSpam(CBaseEntity* pLocal);
	void ExtendFreeze(CBaseEntity* pLocal);
	void Freecam(CUserCmd* pCmd, CBaseEntity* pLocal);
	void RageRetry(CBaseEntity* pLocal);
	void AntiBackstab(CBaseEntity* pLocal, CUserCmd* pCmd);
	void LegJitter(CUserCmd* pCmd, CBaseEntity* pLocal);
	void ViewmodelFlip(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoPeek(CUserCmd* pCmd, CBaseEntity* pLocal);
	void StopMovement(CUserCmd* pCmd, bool *pSendPacket);

	void AntiAFK(CUserCmd* pCmd);
	void ChatSpam();
	void CheatsBypass();
	void PingReducer();
	void WeaponSway();
	void DetectChoke();
	void ChokeCheck(bool* pSendPacket);
	void DoubletapPacket(bool* pSendPacket);
	void EdgeJump(CUserCmd* pCmd, const int nOldGroundEnt);

	void PrintProjAngles(CBaseEntity* pLocal);

	// Late
	void FastStop(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoRocketJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void AutoScoutJump(CUserCmd* pCmd, CBaseEntity* pLocal);
	void FastAccel(CUserCmd* pCmd, CBaseEntity* pLocal, bool* pSendPacket);
	float m_flSpinYaw = 0.f;
	
	bool SteamCleared = false;
public:
	bool TauntControl(CUserCmd* pCmd);
	void RunPre(CUserCmd* pCmd, bool* pSendPacket);
	void RunMid(CUserCmd* pCmd, const int nOldGroundEnt);
	void RunPost(CUserCmd* pCmd, bool* pSendPacket);

	void SteamRPC();
	void UnlockAchievements();
	void LockAchievements();

	bool bAntiWarp = false;
	bool bFastAccel = false;
	bool bMovementStopped = false;

	Vec3 PeekReturnPos;
};

ADD_FEATURE(CMisc, Misc)

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
	                       float time = Vars::Visuals::NotificationLifetime.Value)
	{
		NotificationTexts.push_back(std::make_shared<NotifyText>(text, color, time));
	}

	void Think();
};

ADD_FEATURE(CNotifications, Notifications)