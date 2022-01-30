#pragma once

#include "../../SDK/SDK.h"

class CMisc
{
private:
	void AutoJump(CUserCmd* pCmd);
	void AutoStrafe(CUserCmd* pCmd);
	void NoiseMakerSpam();
	void InitSpamKV(void* pKV);
	void ChatSpam();
	bool steamCleared = false;

public:
	void Run(CUserCmd* pCmd);
	void CheatsBypass();
	void ServerHitbox();
	void EdgeJump(CUserCmd* pCmd, const int nOldFlags);
	void BypassPure();
	void AutoRocketJump(CUserCmd* pCmd);
	void NoPush();
	void SteamRPC();
	std::vector<std::string> strings;
};

inline CMisc g_Misc;

#define MAX_NOTIFY_SIZE 8

class NotifyText {
public:
	std::string m_text;
	Color_t		m_color;
	float		m_time;

public:
	NotifyText() {}
	NotifyText(const std::string& text, Color_t Color, float time) : m_text{ text }, m_color{ Color }, m_time{ time } {}
};

class Notify {
private:
	std::vector< std::shared_ptr< NotifyText > > m_notify_text;

public:
	Notify() : m_notify_text{} {}


	__forceinline void Add(const std::string& text, Color_t color = { 255, 255, 255, 255 }, float time = Vars::Visuals::despawnTime.m_Var) { m_notify_text.push_back(std::make_shared< NotifyText >(text, color, time)); }
	void Think();
};

inline Notify g_notify;
