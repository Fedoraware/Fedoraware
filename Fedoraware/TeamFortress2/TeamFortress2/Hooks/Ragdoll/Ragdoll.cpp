#include "Ragdoll.h"

void Ragdoll::Init()
{
	static DWORD dwFn = g_Pattern.Find(_(L"client.dll"), _(L"E8 ? ? ? ? 8D 4F F8 E8 ? ? ? ? 84 C0 74 78")) + 0x1;
	static DWORD actual = ((*(PDWORD)(dwFn)) + dwFn + 0x4);
	Func.Hook(reinterpret_cast<void*>(actual), Hook);
}

#define Offset(type, ent, offset) *reinterpret_cast<type>(ent + offset)

void Bruh(CBaseEntity* pEntity) {
	Offset(bool*, pEntity, 0xC91) = false; // Gib
	Offset(bool*, pEntity, 0xC92) = false; // Burning
	Offset(bool*, pEntity, 0xC93) = false; // Electrocuted
	Offset(bool*, pEntity, 0xC99) = false; // Become ash
	Offset(bool*, pEntity, 0xCA0) = false; // Gold
	Offset(bool*, pEntity, 0xCA1) = false; // Ice
}

//C_TFRagdoll::CreateTFRagdoll

void __fastcall Ragdoll::Hook(void* ecx, void* edx)
{
	if (const auto& pEntity = reinterpret_cast<CBaseEntity*>(ecx)) {
		if (Vars::Visuals::RagdollEffects::EnemyOnly.m_Var) {
			if (const auto& pLocal = g_EntityCache.m_pLocal) {
				if (Offset(int*, pEntity, 0xCBC) == pLocal->GetTeamNum()) { //Team offset
					return;
				}
			}
		}

		Bruh(pEntity);

		if (Vars::Visuals::RagdollEffects::Burning.m_Var) {
			Offset(bool*, pEntity, 0xC92) = true;
		}

		if (Vars::Visuals::RagdollEffects::Electrocuted.m_Var) {
			Offset(bool*, pEntity, 0xC93) = true;
		}

		if (Vars::Visuals::RagdollEffects::Dissolve.m_Var) {
			Offset(bool*, pEntity, 0xC99) = true;
		}

		if (Vars::Visuals::RagdollEffects::Gold.m_Var) {
			Offset(bool*, pEntity, 0xCA0) = true;
		}

		if (Vars::Visuals::RagdollEffects::Ice.m_Var) {
			Offset(bool*, pEntity, 0xCA1) = true;
		}
	}

	Func.Original<fn>()(ecx, edx);
}
