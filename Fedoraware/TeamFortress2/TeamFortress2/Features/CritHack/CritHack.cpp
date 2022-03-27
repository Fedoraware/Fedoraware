#include "CritHack.h"

int CCritHack::NextCritTick(const CUserCmd* pCmd, int pLoops = 4096)
{
	static int previousCrit = -1;
	static int previousWeapon = -1;

	const auto& pLocal = g_EntityCache.m_pLocal;
	if (!pLocal) { return -1; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return -1; }

	if (previousWeapon == pWeapon->GetIndex() && previousCrit >= pCmd->command_number) { return previousCrit; }

	const int seedBackup = *g_Interfaces.RandomSeed;
	for (int i = 0; i < pLoops; i++) {
		const int cmdNum = pCmd->command_number + i;
		*g_Interfaces.RandomSeed = MD5_PseudoRandom(cmdNum) & std::numeric_limits<int>::max();


		const bool isCrit = pWeapon->CalcIsAttackCriticalHelper();

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

	if (!GetAsyncKeyState(Vars::CritHack::CritKey.m_Var)) { return; }

	if (pCmd->buttons & IN_ATTACK)
	{
		const int nextCrit = NextCritTick(pCmd);
		if (nextCrit != -1)
		{
			pCmd->command_number = nextCrit;
			pCmd->random_seed = MD5_PseudoRandom(nextCrit) & std::numeric_limits<int>::max();
		}
	}
}

void CCritHack::Draw()
{
	if (!Vars::CritHack::Active.m_Var) { return; }

	if (!g_Interfaces.Engine->IsInGame()) { return; }

	if (const auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (!pLocal->IsAlive()) { return; }

		const auto& pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon) { return; }

		const stats_t stats = *reinterpret_cast<stats_t*>(pWeapon + 0xA54);
		const int bucket = static_cast<int>(stats.flCritBucket);
		
		g_Draw.String(FONT_MENU, g_ScreenSize.c, (g_ScreenSize.h / 2) + 320, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, (std::string("Bucket: ") + std::to_string(bucket)).c_str());
	}
}