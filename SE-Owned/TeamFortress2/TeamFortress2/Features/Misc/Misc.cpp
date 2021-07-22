#include "Misc.h"

#include "../Vars.h"

void CMisc::Run(CUserCmd* pCmd)
{
	AutoJump(pCmd);
	AutoStrafe(pCmd);
	NoiseMakerSpam();
	ChatSpam();
}

void CMisc::EdgeJump(CUserCmd* pCmd, const int nOldFlags)
{
	if ((nOldFlags & FL_ONGROUND) && Vars::Misc::EdgeJump.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsAlive() && !pLocal->IsOnGround() && !pLocal->IsSwimming())
				pCmd->buttons |= IN_JUMP;
		}
	}
}

void CMisc::AutoJump(CUserCmd *pCmd)
{
	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (!Vars::Misc::AutoJump.m_Var
			|| !pLocal->IsAlive()
			|| pLocal->IsSwimming()
			|| pLocal->IsInBumperKart()
			|| pLocal->IsAGhost())
			return;

		static bool bJumpState = false;

		if (pCmd->buttons & IN_JUMP)
		{
			if (!bJumpState && !pLocal->IsOnGround())
				pCmd->buttons &= ~IN_JUMP;

			else if (bJumpState)
				bJumpState = false;
		}

		else if (!bJumpState)
			bJumpState = true;
	}
}

void CMisc::AutoStrafe(CUserCmd* pCmd)
{
	if (Vars::Misc::AutoStrafe.m_Var)
	{
		if (const auto& pLocal = g_EntityCache.m_pLocal)
		{
			if (pLocal->IsAlive() && !pLocal->IsSwimming() && !pLocal->IsOnGround() && (pCmd->mousedx > 1 || pCmd->mousedx < -1))
				pCmd->sidemove = pCmd->mousedx > 1 ? 450.f : -450.f;
		}
	}
}

void CMisc::InitSpamKV(void* pKV)
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);

	static DWORD dwHashFunctionLocation = g_Pattern.Find(_(L"client.dll"), _(L"FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	static HashFunc_t SymbForString = (HashFunc_t)* *(PDWORD*)(dwHashFunctionLocation + 0x2);

	int nAddr = 0;
	while (nAddr < 29)
	{
		switch (nAddr)
		{
			case 0:
				*(PDWORD)((DWORD)pKV + nAddr) = SymbForString(chCommand, true);
				break;
			case 16:
				*(PDWORD)((DWORD)pKV + nAddr) = 0x10000;
				break;
			default:
				*(PDWORD)((DWORD)pKV + nAddr) = 0;
				break;
		}

		nAddr += 4;
	}
}

void CMisc::NoiseMakerSpam()
{
	if (!Vars::Misc::NoisemakerSpam.m_Var)
		return;

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetUsingActionSlot())
			return;

		if (pLocal->GetNextNoiseMakerTime() < g_Interfaces.GlobalVars->curtime)
		{
			if (const auto pKV = Utils::InitKeyValue())
			{
				InitSpamKV(pKV);
				g_Interfaces.Engine->ServerCmdKeyValues(pKV);
			}
		}
	}
}

void CMisc::BypassPure()
{
	if (Vars::Misc::BypassPure.m_Var)
	{
		static DWORD dwAddress = 0x0;

		while (!dwAddress)
			dwAddress = g_Pattern.Find(_(L"engine.dll"), _(L"A1 ? ? ? ? 56 33 F6 85 C0"));

		static DWORD* pPure = nullptr;

		while (!pPure)
		{
			if (reinterpret_cast<DWORD**>(dwAddress + 0x01))
				pPure = *reinterpret_cast<DWORD**>(dwAddress + 0x01);
		}

		if (*pPure)
			*pPure = 0x0;
	}
}

std::string GetSpam(const int nIndex) {
	std::string str;

	switch (nIndex)
	{
		case 0: str = XorStr("say SEOwned - Available for free @ unknowncheats.me!").str(); break;
		case 1: str = XorStr("say SEOwned - Better than 20$ Darkstorm!").str(); break;
		case 2: str = XorStr("say SEOwned - Go get yours now from unknowncheats.me!").str(); break;
		case 3: str = XorStr("say SEOwned - Premium cheat by spook953 and Lak3, but it's free!").str(); break;
		default: str = XorStr("say SEOwned - See you @ unknowncheats.me!").str(); break;
	}

	return str;
}

void CMisc::ChatSpam()
{
	if (!Vars::Misc::ChatSpam.m_Var)
		return;

	float flCurTime = g_Interfaces.Engine->Time(); 
	static float flNextSend = 0.0f;

	if (flCurTime > flNextSend) {
		g_Interfaces.Engine->ClientCmd_Unrestricted(GetSpam(Utils::RandIntSimple(0, 5)).c_str());
		flNextSend = (flCurTime + 3.0f);
	}
}

void CMisc::AutoRocketJump(CUserCmd *pCmd)
{
	if (!Vars::Misc::AutoRocketJump.m_Var || !g_GlobalInfo.m_bWeaponCanAttack || !GetAsyncKeyState(VK_RBUTTON))
		return;

	if (g_Interfaces.EngineVGui->IsGameUIVisible() || g_Interfaces.Surface->IsCursorVisible())
		return;

	if (const auto &pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->GetClassNum() != CLASS_SOLDIER || !pLocal->IsOnGround() || pLocal->IsDucking())
			return;

		if (const auto &pWeapon = g_EntityCache.m_pLocalWeapon)
		{
			if (pWeapon->IsInReload()) {
				pCmd->buttons |= IN_ATTACK;
				return;
			}

			else
			{
				if (pCmd->buttons & IN_ATTACK)
					pCmd->buttons &= ~IN_ATTACK;
			}

			if (g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheBeggarsBazooka
				|| g_GlobalInfo.m_nCurItemDefIndex == Soldier_m_TheCowMangler5000
				|| pWeapon->GetSlot() != SLOT_PRIMARY)
				return;

			if (pLocal->GetViewOffset().z < 45.05f)
			{
				pCmd->buttons |= IN_ATTACK | IN_JUMP;

				Vec3 vVelocity = pLocal->GetVelocity();
				Vec3 vAngles = { vVelocity.IsZero() ? 89.0f : 45.0f, Math::VelocityToAngles(vVelocity).y - 180.0f, 0.0f };

				if (g_GlobalInfo.m_nCurItemDefIndex != Soldier_m_TheOriginal && !vVelocity.IsZero()) {
					Vec3 vForward = {}, vRight = {}, vUp = {};
					Math::AngleVectors(vAngles, &vForward, &vRight, &vUp);
					Math::VectorAngles((vForward * 23.5f) + (vRight * -5.6f) + (vUp * -3.0f), vAngles);
				}

				Math::ClampAngles(vAngles);
				pCmd->viewangles = vAngles;
				g_GlobalInfo.m_bSilentTime = true;
			}

			else pCmd->buttons |= IN_DUCK;
		}
	}
}