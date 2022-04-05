#include "Viewmodel.h"

void __fastcall Viewmodel::Hook(void* ecx, void* edx, CBaseEntity* owner, Vec3& eyePosition, Vec3& eyeAngles)
{
	static Vec3 m_vEyeAngDelayed;
	static auto originalFn = Func.Original<fn>();
	if (auto& pLocal = g_EntityCache.m_pLocal)
	{
		if (pLocal->IsAlive() && !g_GlobalInfo.m_vAimPos.IsZero() && Vars::Visuals::AimbotViewmodel.m_Var)
		{
			if (g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE)
			{
				eyeAngles = Math::CalcAngle(eyePosition, g_GlobalInfo.m_vPredictedPos);
			}
			else
			{
				eyeAngles = Math::CalcAngle(eyePosition, g_GlobalInfo.m_vAimPos);
			}
			m_vEyeAngDelayed = eyeAngles;
			g_GlobalInfo.vEyeAngDelay = 0;
		}
		else if (pLocal->IsAlive())
		{
			if (g_GlobalInfo.vEyeAngDelay < 32) { eyeAngles = m_vEyeAngDelayed; }
			// looks hot ty senator for the idea
			else { eyeAngles = g_Interfaces.Engine->GetViewAngles(); }
		}
	}
	//VM Offsets

	Vec3 vForward = {}, vRight = {}, vUp = {};
	Math::AngleVectors(eyeAngles, &vForward, &vRight, &vUp);

	// viewmodel offsets should probably be locked between 2 values, just like we should clamp our eye angles, however, I don't want to, f*ck you

	Vec3 vEyePosition = eyePosition + (
		(vRight * Vars::Visuals::VMOffsets.x) +
		(vForward * Vars::Visuals::VMOffsets.y) +
		(vUp * Vars::Visuals::VMOffsets.z)
	);

	eyeAngles.z += Vars::Visuals::VMRoll.m_Var; //VM Roll

	originalFn(ecx, edx, owner, vEyePosition, eyeAngles);
}

void Viewmodel::Init()
{
	auto Src = reinterpret_cast<void*>(g_Pattern.Find(
		L"client.dll",
		L"55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57"));
	Func.Hook(Src, Hook);
}
