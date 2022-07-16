#include "../Hooks.h"

#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/AntiHack/AntiAim.h"
#include "../../Features/AntiHack/FakeLag/FakeLag.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"
#include "../../Features/Camera/CameraWindow.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Fedworking/Fedworking.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Followbot/Followbot.h"
#include "../../Features/Vars.h"
#include "../../Features/Discord/Discord.h"

#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Menu/MaterialEditor/MaterialEditor.h"

#include "../../SDK/Discord/include/discord_rpc.h"

void UpdateRichPresence()
{
	F::DiscordRPC.Update();
	F::Misc.SteamRPC();
}

void AppendCache()
{
	CBaseEntity* pLocal = g_EntityCache.GetLocal();
	const int tickcount = I::GlobalVars->tickcount;

	for (CBaseEntity* pCaching : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
	{
		std::unordered_map<int, PlayerCache>& openCache = G::Cache[pCaching];
		if (pCaching == pLocal || pCaching->GetDormant())
		{
			openCache.clear();
			continue;
		}

		if (openCache.size() > round(1.f / I::GlobalVars->interval_per_tick))
		{
			openCache.erase(openCache.begin()); // delete the first value if our cache lasts longer than a second.
		}

		openCache[tickcount].m_vecOrigin = pCaching->m_vecOrigin();
		openCache[tickcount].m_vecVelocity = pCaching->m_vecVelocity();
		openCache[tickcount].eyePosition = pCaching->GetEyeAngles();
		openCache[tickcount].playersPredictedTick = TIME_TO_TICKS(pCaching->GetSimulationTime());
	}
}

MAKE_HOOK(ClientModeShared_CreateMove, Utils::GetVFuncPtr(I::ClientMode, 21), bool, __fastcall,
          void* ecx, void* edx, float input_sample_frametime, CUserCmd* pCmd)
{
	G::SilentTime = false;
	G::IsAttacking = false;
	G::FakeShotPitch = false;

	if (!pCmd || !pCmd->command_number)
	{
		return Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd);
	}

	if (Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd))
	{
		I::Prediction->SetLocalViewAngles(pCmd->viewangles);
	}

	// Get the pointer to pSendPacket
	uintptr_t _bp;
	__asm mov _bp, ebp;
	auto pSendPacket = reinterpret_cast<bool*>(***reinterpret_cast<uintptr_t***>(_bp) - 0x1);

	int nOldFlags = 0;
	Vec3 vOldAngles = pCmd->viewangles;
	float fOldSide = pCmd->sidemove;
	float fOldForward = pCmd->forwardmove;

	G::CurrentUserCmd = pCmd;

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		nOldFlags = pLocal->GetFlags();

		// Update Discord/Steam rich presence every second
		static Timer richPresenceTimer{};
		if (richPresenceTimer.Run(1000))
		{
			UpdateRichPresence();
		}

		if (const int MaxSpeed = pLocal->GetMaxSpeed()) {
			G::Frozen = MaxSpeed == 1;
		}

		// Update Global Info
		if (const auto& pWeapon = g_EntityCache.GetWeapon())
		{
			const int nItemDefIndex = pWeapon->GetItemDefIndex();

			if (G::CurItemDefIndex != nItemDefIndex || !pWeapon->GetClip1() || (!pLocal->IsAlive() || pLocal->IsTaunting() || pLocal->IsBonked() || pLocal->IsAGhost() || pLocal->IsInBumperKart()))
			{
				G::WaitForShift = DT_WAIT_CALLS;
			}

			G::CurItemDefIndex = nItemDefIndex;
			G::WeaponCanHeadShot = pWeapon->CanWeaponHeadShot();
			G::WeaponCanAttack = pWeapon->CanShoot(pLocal);
			G::WeaponCanSecondaryAttack = pWeapon->CanSecondaryAttack(pLocal);
			G::CurWeaponType = Utils::GetWeaponType(pWeapon);
			G::IsAttacking = Utils::IsAttacking(pCmd, pWeapon);

			if (pWeapon->GetSlot() != SLOT_MELEE)
			{
				if (pWeapon->IsInReload())
				{
					G::WeaponCanAttack = true;
				}

				if (G::CurItemDefIndex != Soldier_m_TheBeggarsBazooka)
				{
					if (pWeapon->GetClip1() == 0)
					{
						G::WeaponCanAttack = false;
					}
				}
			}
		}

		if (Vars::Misc::CL_Move::RechargeWhileDead.Value)
		{
			if (!pLocal->IsAlive() && G::ShiftedTicks)
			{
				G::RechargeQueued = true;
			}
		}

		
		if (Vars::Misc::CL_Move::AutoRecharge.Value && !G::ShouldShift && !G::Recharging && !G::ShiftedTicks)
		{
			if (pLocal->GetVecVelocity().Length2D() < 5.0f && !(pCmd->buttons & IN_ATTACK))
			{
				G::RechargeQueued = true;
			}
		}

		if (const INetChannel* netChannel = I::Engine->GetNetChannelInfo())
		{
			static const char* oServerAddress = netChannel->GetAddress();
			const char* cServerAddress = netChannel->GetAddress();

			static const char* oMap = I::Engine->GetLevelName();
			const char* cMap = I::Engine->GetLevelName();

			if (oServerAddress != cServerAddress || oMap != cMap)
			{
				oServerAddress = cServerAddress;
				oMap = cMap;
				G::LoadInCount++;
				G::NextSafeTick = 0;
			}
		}
	}

	// Run Features
	{
		F::Misc.Run(pCmd);
		F::Fedworking.Run();
		F::CameraWindow.Update();
		F::BadActors.OnTick();

		F::EnginePrediction.Start(pCmd);
		{
			F::Backtrack.Run(pCmd);
			F::Aimbot.Run(pCmd);
			F::Auto.Run(pCmd);
			F::AntiAim.Run(pCmd, pSendPacket);
			F::Misc.EdgeJump(pCmd, nOldFlags);
		}
		F::EnginePrediction.End(pCmd);

		F::CritHack.Run(pCmd);
		F::Misc.RunLate(pCmd);
		F::Resolver.Update(pCmd);
		F::Followbot.Run(pCmd);
		F::FakeLag.OnTick(pCmd, pSendPacket);
	}

	AppendCache(); // hopefully won't cause issues.
	G::ViewAngles = pCmd->viewangles;

	// Recharge doubletap by shifting packets
	static int nChoked = 0;
	if (G::ShouldShift)
	{
		*pSendPacket = G::ShiftedTicks == 1;

		if (!*pSendPacket) { nChoked++; }
		else { nChoked = 0; }
		if (nChoked > 21) { *pSendPacket = true; }
	}

	// Check if Fakelag Chams should be drawn
	if (G::ShiftedTicks > 0)
	{
		F::FakeAng.DrawChams = false;
	}

	// Party Crasher: Crashes the party by spamming messages
	if (Vars::Misc::PartyCrasher.Value)
	{
		I::Engine->ClientCmd_Unrestricted("tf_party_chat \"FED@MA==\"");
	}

	// Handle Taunt Slide
	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (Vars::Misc::TauntSlide.Value && pLocal->IsTaunting())
		{
			if (Vars::Misc::TauntControl.Value)
			{
				pCmd->viewangles.x = (pCmd->buttons & IN_BACK)
					                     ? 91.0f
					                     : (pCmd->buttons & IN_FORWARD)
					                     ? 0.0f
					                     : 90.0f;
			}

			return false;
		}

		if (const ConVar* debugMode = I::CVars->FindVar("debugMode"))
		{
			Vars::Debug::DebugInfo.Value = Vars::Debug::DebugBool.Value = debugMode->GetInt();
		}
	}

	// Validates the cham materials every 3 seconds
	static Timer validateTimer{};
	if (validateTimer.Run(3000)) //
	{
		for (IMaterial* curMat : F::DMEChams.v_MatListGlobal)
		{
			if (!curMat) { continue; }
			F::DMEChams.ValidateMaterial(curMat);
		}
	}

	// Handle Silent Time
	static bool bWasSet = false;
	if (G::SilentTime)
	{
		*pSendPacket = false;
		bWasSet = true;
	}
	else
	{
		if (bWasSet)
		{
			*pSendPacket = true;
			pCmd->viewangles = vOldAngles;
			pCmd->sidemove = fOldSide;
			pCmd->forwardmove = fOldForward;
			bWasSet = false;
		}
	}

	G::EyeAngDelay++; // Used for the return delay in the viewmodel aimbot
	G::LastUserCmd = pCmd;

	if (G::ForceSendPacket)
	{
		*pSendPacket = true;
		G::ForceSendPacket = false;
	} // if we are trying to force update do this lol
	else if (G::ForceChokePacket)
	{
		*pSendPacket = false;
		G::ForceChokePacket = false;
	} // check after force send to prevent timing out possibly

	// Stop movement if required
	if (G::ShouldStop || (G::RechargeQueued || G::Recharging && Vars::Misc::CL_Move::StopMovement.Value))
	{
		//G::ShouldStop = false;	//	we still need to stop if we didn't stop...
		Utils::StopMovement(pCmd, !G::ShouldShift);
		if (!G::IsAttacking && !G::Recharging && !G::ShouldStop) {	//	only do this code if we DID actually stop.
			*pSendPacket = false;	//	stop angle shit
		}
		return false;
	}

	// do this at the end just in case aimbot / triggerbot fired.//
	if (const auto& pWeapon = g_EntityCache.GetWeapon()) {
		if (pCmd->buttons & IN_ATTACK && Vars::Misc::CL_Move::SafeTick.Value) {
			if (G::NextSafeTick > I::GlobalVars->tickcount && G::ShouldShift && G::ShiftedTicks) {
				pCmd->buttons &= ~IN_ATTACK;
			}
			else {
				G::NextSafeTick = I::GlobalVars->tickcount + g_ConVars.sv_maxusrcmdprocessticks_holdaim->GetInt();
			}
		}
	}

	if (G::SilentTime ||
		G::AAActive ||
		G::FakeShotPitch ||
		G::HitscanSilentActive ||
		G::AvoidingBackstab ||
		G::ProjectileSilentActive ||
		G::RollExploiting)
	{
		return false;
	}

	return Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd);
}
