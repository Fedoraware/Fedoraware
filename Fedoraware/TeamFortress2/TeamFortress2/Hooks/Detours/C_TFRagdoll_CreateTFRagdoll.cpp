#include "../Hooks.h"

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

MAKE_HOOK(C_TFRagdoll_CreateTFRagdoll, g_Pattern.E8(L"client.dll", L"E8 ? ? ? ? 8D 4F F8 E8 ? ? ? ? 84 C0 74 78"), void, __fastcall,
		  void* ecx, void* edx)
{
	if (Vars::Visuals::RemoveRagdolls.Value) { return; }

	if (const auto& pEntity = static_cast<CBaseEntity*>(ecx))
	{
		if (Vars::Visuals::RagdollEffects::EnemyOnly.Value)
		{
			if (const auto& pLocal = g_EntityCache.GetLocal())
			{
				if (Offset(int*, pEntity, 0xCBC) == pLocal->GetTeamNum())
				{
					//Team offset
					return;
				}
			}
		}

		ClearEffects(pEntity);

		Offset(bool*, pEntity, 0xC92) = Vars::Visuals::RagdollEffects::Burning.Value;
		Offset(bool*, pEntity, 0xC93) = Vars::Visuals::RagdollEffects::Electrocuted.Value;
		Offset(bool*, pEntity, 0xC99) = Vars::Visuals::RagdollEffects::BecomeAsh.Value;
		Offset(bool*, pEntity, 0xC95) = Vars::Visuals::RagdollEffects::Dissolve.Value;
		Offset(bool*, pEntity, 0xCA0) = Vars::Visuals::RagdollEffects::RagdollType.Value == 1;
		Offset(bool*, pEntity, 0xCA1) = Vars::Visuals::RagdollEffects::RagdollType.Value == 2;
	}

	Hook.Original<FN>()(ecx, edx);
}
