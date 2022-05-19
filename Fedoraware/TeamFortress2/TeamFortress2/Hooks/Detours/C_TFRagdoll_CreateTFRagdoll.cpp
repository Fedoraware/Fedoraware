#include "../Hooks.h"

DWORD CreateTFRagdollAddress()
{
	static DWORD dwFn = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 8D 4F F8 E8 ? ? ? ? 84 C0 74 78") + 0x1;
	return *(PDWORD)dwFn + dwFn + 0x4;
}

#define Offset(type, ent, offset) *reinterpret_cast<type>(ent + offset)

void ClearEffects(CBaseEntity* pEntity)
{
	Offset(bool*, pEntity, 0xC91) = false; // Gib
	Offset(bool*, pEntity, 0xC92) = false; // Burning
	Offset(bool*, pEntity, 0xC93) = false; // Electrocuted
	Offset(bool*, pEntity, 0xC99) = false; // Become ash
	Offset(bool*, pEntity, 0xCA0) = false; // Gold
	Offset(bool*, pEntity, 0xCA1) = false; // Ice
}

MAKE_HOOK(C_TFRagdoll_CreateTFRagdoll, CreateTFRagdollAddress(), void, __fastcall,
		  void* ecx, void* edx)
{
	if (const auto& pEntity = static_cast<CBaseEntity*>(ecx))
	{
		if (Vars::Visuals::RagdollEffects::EnemyOnly.m_Var)
		{
			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (Offset(int*, pEntity, 0xCBC) == pLocal->GetTeamNum())
				{
					//Team offset
					return;
				}
			}
		}

		ClearEffects(pEntity);

		Offset(bool*, pEntity, 0xC92) = Vars::Visuals::RagdollEffects::Burning.m_Var;
		Offset(bool*, pEntity, 0xC93) = Vars::Visuals::RagdollEffects::Electrocuted.m_Var;
		Offset(bool*, pEntity, 0xC99) = Vars::Visuals::RagdollEffects::BecomeAsh.m_Var;
		Offset(bool*, pEntity, 0xC95) = Vars::Visuals::RagdollEffects::Dissolve.m_Var;
		Offset(bool*, pEntity, 0xCA0) = Vars::Visuals::RagdollEffects::RagdollType.m_Var == 1;
		Offset(bool*, pEntity, 0xCA1) = Vars::Visuals::RagdollEffects::RagdollType.m_Var == 2;
	}

	Hook.Original<FN>()(ecx, edx);
}
