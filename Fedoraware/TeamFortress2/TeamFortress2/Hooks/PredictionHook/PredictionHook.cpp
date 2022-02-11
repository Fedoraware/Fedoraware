#include "PredictionHook.h"
#include "../../Features/Crits/Crits.h"

int CalculateTick(int simTicks, CBaseEntity* player)
{
	int clockcorrect = TIME_TO_TICKS(0.06f); //sv_clockcorrectmsecs
	int nIdealFinalTick = g_Interfaces.GlobalVars->tickcount + TIME_TO_TICKS(
		g_Interfaces.Engine->GetNetChannelInfo()->GetLatency(0)) + clockcorrect;
	int EstimatedFinal = player->GetTickBase() + simTicks;
	int fast = nIdealFinalTick + clockcorrect;
	int slow = nIdealFinalTick - clockcorrect;
	if (EstimatedFinal > fast || EstimatedFinal < slow) { return nIdealFinalTick - simTicks; }
	// this is useless
	return EstimatedFinal;
}

// Server gives us garbage so let's just calc our own
static float getObservedCritChance()
{
	if (!(g_Crits.cachedDamage - g_Crits.roundDamage))
		return 0.0f;
	// Same is used by server
	float normalized_damage = (float)g_Crits.critDamage / 3.0f;
	return normalized_damage / (normalized_damage + (float)((g_Crits.cachedDamage - g_Crits.roundDamage) - g_Crits.critDamage));
}


// Fix observed crit chance
void fixObservedCritchance(CBaseCombatWeapon* weapon)
{
	weapon_info info(weapon);
	info.m_flObservedCritChance = getObservedCritChance();
	info.RestoreData(weapon);
}

static weapon_info last_weapon_info;

// Fix bucket on non-local servers
void fixBucket(CBaseCombatWeapon* weapon, CUserCmd* cmd)
{
	INetChannel* ch = (INetChannel*)g_Interfaces.Engine->GetNetChannelInfo();
	if (!ch)
		return;

	static int last_weapon;
	// This tracks only when bucket is updated
	static int last_update_command;

	fixObservedCritchance(weapon);

	weapon_info original_info(weapon);

	weapon_info info(weapon);

	// Changed bucket more than once this tick, this is wrong and needs to be corrected.
	if (weapon->GetIndex() == last_weapon && info != last_weapon_info && last_update_command == cmd->command_number)
		info = last_weapon_info;

	last_weapon = weapon->GetIndex();
	// Bucket changed, update
	if (last_weapon_info.flCritBucket != original_info.flCritBucket)
		last_update_command = cmd->command_number;

	last_weapon_info = info;

	info.RestoreData(weapon);
}

void __stdcall PredictionHook::RunCommand::Hook(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveHelper* pMoveHelper)
{
	if (pMoveHelper && !g_Interfaces.MoveHelper)
		g_Interfaces.MoveHelper = pMoveHelper;
	
	auto& pLocal = g_EntityCache.m_pLocal;

	if (pEntity && pEntity->IsAlive() && pLocal && pLocal->IsAlive() && pLocal == pEntity && pCmd && pCmd->command_number)
	{
		int BackupTick = pEntity->GetTickBase();
		float CurtimeBackup = g_Interfaces.GlobalVars->curtime;

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift) {
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(CalculateTick(g_Interfaces.ClientState->chokedcommands + g_GlobalInfo.m_nShifted + 1,
					pEntity));
				g_Interfaces.GlobalVars->curtime = TICKS_TO_TIME(pEntity->GetTickBase());
			}
		}

		Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);

		fixBucket(g_EntityCache.m_pLocalWeapon, pCmd);

		if (Vars::Misc::CL_Move::Doubletap.m_Var && g_GlobalInfo.m_bShouldShift) {
			if (pCmd->command_number == g_GlobalInfo.lateUserCmd->command_number)
			{
				pEntity->SetTickBase(BackupTick);
				g_Interfaces.GlobalVars->curtime = CurtimeBackup;
			}
		}
	} else {
		Table.Original<fn>(index)(g_Interfaces.Prediction, pEntity, pCmd, pMoveHelper);
	}



	


}
