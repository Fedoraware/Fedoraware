#include "CritHack.h"

#define MASK_SIGNED 0x7FFFFFFF

int CCritHack::NextCritTick(CUserCmd* pCmd, int pLoops = 4096)
{
	static int previousCrit = -1;
	static int previousWeapon = -1;

	const auto& pLocal = g_Interfaces.EntityList->GetClientEntity(g_Interfaces.Engine->GetLocalPlayer());
	if (!pLocal) { return -1; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return -1; }

	if (previousWeapon == pWeapon->GetIndex() && previousCrit >= pCmd->command_number) { return -1; }

	int seedBackup = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
	for (int i = 0; i < pLoops; i++) {
		int cmdNum = pCmd->command_number + i;
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;

		bool isCrit = pWeapon->CalcIsAttackCriticalHelper();
		if (isCrit) {
			*g_Interfaces.RandomSeed = seedBackup;
			previousCrit = cmdNum;
			previousWeapon = pWeapon->GetIndex();
			return cmdNum;
		}
	}

	*g_Interfaces.RandomSeed = seedBackup;
	return -1;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!Vars::CritHack::Active.m_Var) { return; }

	if (!(pCmd->buttons & IN_ATTACK)) { return; }

	if (GetAsyncKeyState(Vars::CritHack::CritKey.m_Var))
	{
		int nextCrit = NextCritTick(pCmd);
		if (nextCrit != -1)
		{
			pCmd->command_number = nextCrit;
			pCmd->random_seed = MD5_PseudoRandom(nextCrit) & MASK_SIGNED;
		}
	}
}

void CCritHack::Draw()
{
	
}