#include "../Hooks.h"

#include "../../Features/Prediction/Prediction.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Auto/Auto.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"
#include "../../Features/Camera/CameraWindow.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Fedworking/Fedworking.h"
#include "../../Features/Resolver/Resolver.h"
#include "../../Features/AntiHack/CheaterDetection/CheaterDetection.h"
#include "../../Features/Followbot/Followbot.h"
#include "../../Features/Vars.h"
#include "../../Features/Aimbot/AimbotGlobal/AimbotGlobal.h"
#include "../../Features/Menu/MaterialEditor/MaterialEditor.h"
#include "../../Features/TickHandler/TickHandler.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/NoSpread/NoSpread.h"

void AttackingUpdate()
{
	if (!F::AimbotGlobal.IsAttacking()) { return; }

	if (const auto& pLocal = g_EntityCache.GetLocal())
	{
		if (const auto& pWeapon = g_EntityCache.GetWeapon())
		{
			const float flFireDelay = pWeapon->GetWeaponData().m_flTimeFireDelay;
			pWeapon->m_flNextPrimaryAttack() = static_cast<float>(pLocal->GetTickBase()) * I::GlobalVars->interval_per_tick + flFireDelay;
		}
	}
}

MAKE_HOOK(ClientModeShared_CreateMove, Utils::GetVFuncPtr(I::ClientModeShared, 21), bool, __fastcall,
		  void* ecx, void* edx, float input_sample_frametime, CUserCmd* pCmd)
{
	G::UpdateView = true;
	G::SilentTime = false;
	F::AimbotGlobal.SetAttacking(false);

	if (!pCmd || !pCmd->command_number) { return Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd); }
	if (Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd)) { I::Prediction->SetLocalViewAngles(pCmd->viewangles); }

	// Get the pointer to pSendPacket
	uintptr_t _bp;
	__asm mov _bp, ebp;
	auto pSendPacket = reinterpret_cast<bool*>(***reinterpret_cast<uintptr_t***>(_bp) - 0x1);

	//	save old info
	static int nOldFlags = 0;
	static int nOldGroundEnt = 0;
	static Vec3 vOldAngles = pCmd->viewangles;
	static float fOldSide = pCmd->sidemove;
	static float fOldForward = pCmd->forwardmove;

	G::CurrentUserCmd = pCmd;

	if (!G::ShouldShift)
	{
		if (const auto& pLocal = g_EntityCache.GetLocal())
		{
			nOldFlags = pLocal->GetFlags();
			nOldGroundEnt = pLocal->m_hGroundEntity();

			if (const int MaxSpeed = pLocal->GetMaxSpeed()) {
				G::Frozen = MaxSpeed == 1;
			}

			// Update Global Info
			if (const auto& pWeapon = g_EntityCache.GetWeapon()) {
				const int nItemDefIndex = pWeapon->GetItemDefIndex();

				G::WeaponCanAttack = pWeapon->CanShoot(pLocal);

				if (pWeapon->GetSlot() != SLOT_MELEE) {
					if (pWeapon->IsInReload()) {
						G::WeaponCanAttack = true;
					}

					if (G::CurItemDefIndex != Soldier_m_TheBeggarsBazooka) {
						if (pWeapon->GetClip1() == 0) {
							G::WeaponCanAttack = false;
						}
					}
				}

				if (G::CurItemDefIndex != nItemDefIndex || !G::WeaponCanAttack) {
					G::WaitForShift = DT_WAIT_CALLS;
				}

				G::CurItemDefIndex = nItemDefIndex;
				G::WeaponCanHeadShot = pWeapon->CanWeaponHeadShot();
				G::WeaponCanSecondaryAttack = pWeapon->CanSecondaryAttack(pLocal);
				G::CurWeaponType = Utils::GetWeaponType(pWeapon);
				F::AimbotGlobal.SetAttacking(Utils::IsAttacking(pCmd, pWeapon));
			}

			if (!G::ShiftedTicks) {
				G::RechargeQueued = (Vars::Misc::CL_Move::RechargeWhileDead.Value && !pLocal->IsAlive()) || (Vars::Misc::CL_Move::AutoRecharge.Value && pLocal->GetVecVelocity().Length2D() < 5.0f && !(pCmd->buttons));
			}
		}
	}
	else if (const auto& pWeapon = g_EntityCache.GetWeapon())
	{
		if (const auto& pLocal = g_EntityCache.GetLocal())
		{
			G::WeaponCanAttack = pWeapon->CanShoot(pLocal);
			F::AimbotGlobal.SetAttacking(Utils::IsAttacking(pCmd, pWeapon));
		}
	}	//	we always need this :c 

	I::Prediction->Update(I::ClientState->m_nDeltaTick,
		I::ClientState->m_nDeltaTick > 0,
		I::ClientState->last_command_ack,
		I::ClientState->lastoutgoingcommand + I::ClientState->chokedcommands);

	// Run Features
	{
		F::Misc.RunPre(pCmd, pSendPacket);
		F::Fedworking.Run();
		F::CameraWindow.Update();
		F::BadActors.OnTick();
		F::Backtrack.Run(pCmd);

		F::EnginePrediction.Start(pCmd);
		{
			F::Aimbot.Run(pCmd);
			F::Auto.Run(pCmd);
			F::Misc.RunMid(pCmd, nOldGroundEnt);
		}
		F::EnginePrediction.End(pCmd);

		F::Ticks.CreateMove(pCmd);
		F::CritHack.Run(pCmd);
		F::Misc.RunPost(pCmd, pSendPacket);
		F::Resolver.CreateMove();
		F::Followbot.Run(pCmd);
	}

	if (*pSendPacket)
	{
		F::FakeAng.Run(pCmd);
		F::FakeAng.DrawChams = Vars::AntiHack::AntiAim::Active.Value || Vars::Misc::CL_Move::Fakelag.Value;
	}

	G::ViewAngles = pCmd->viewangles;

	// Party Crasher: Crashes the party by spamming messages
	if (Vars::Misc::PartyCrasher.Value && !G::ShouldShift)
	{
		I::EngineClient->ClientCmd_Unrestricted("tf_party_chat \"FED@MA==\"");
	}

	AttackingUpdate();

	// do this at the end just in case aimbot / triggerbot fired.
	if (const auto& pWeapon = g_EntityCache.GetWeapon(); const auto & pLocal = g_EntityCache.GetLocal())
	{
		if (pCmd->buttons & IN_ATTACK && (Vars::Misc::CL_Move::SafeTick.Value || (Vars::Misc::CL_Move::SafeTickAirOverride.Value && !pLocal->OnSolid())))
		{
			if (G::NextSafeTick > I::GlobalVars->tickcount && G::ShouldShift && G::ShiftedTicks)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
			else
			{
				G::NextSafeTick = I::GlobalVars->tickcount + g_ConVars.sv_maxusrcmdprocessticks_holdaim->GetInt() + 1;
			}
		}
	}

	F::NoSpread.CreateMoveProjectile(pCmd);
	F::NoSpread.CreateMoveHitscan(pCmd);

	G::LastUserCmd = pCmd;

	const bool bShouldSkip = (G::SilentTime || G::AAActive || G::HitscanSilentActive || G::AvoidingBackstab || !G::UpdateView || !F::Misc.TauntControl(pCmd));
	return bShouldSkip ? false : Hook.Original<FN>()(ecx, edx, input_sample_frametime, pCmd);
}
