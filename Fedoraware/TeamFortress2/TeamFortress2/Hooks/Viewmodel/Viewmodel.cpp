#include "Viewmodel.h"

void __fastcall Viewmodel::Hook(void* ecx, void* edx, CBaseEntity* owner, Vec3& eyePosition, Vec3& eyeAngles) {
    static auto originalFn = Func.Original<fn>();
    if (auto& pLocal = g_EntityCache.m_pLocal) {
        if (pLocal->IsAlive() && !g_GlobalInfo.m_vAimPos.IsZero() && Vars::Visuals::AimbotViewmodel.m_Var) {
            if (g_GlobalInfo.m_WeaponType == EWeaponType::PROJECTILE) {
                eyeAngles = Math::CalcAngle(pLocal->GetEyePosition(), g_GlobalInfo.m_vPredictedPos);
                g_GlobalInfo.m_vEyeAngDelayed = eyeAngles;
            }
            else {
                eyeAngles = Math::CalcAngle(pLocal->GetEyePosition(), g_GlobalInfo.m_vAimPos);
                g_GlobalInfo.m_vEyeAngDelayed = eyeAngles;
            }
            g_GlobalInfo.vEyeAngDelay = 0;
        }
        else if (pLocal->IsAlive())
        {
            if (g_GlobalInfo.vEyeAngDelay < 25) { eyeAngles = g_GlobalInfo.m_vEyeAngDelayed; } // looks fucking hot ty senator for the idea
            else { eyeAngles = g_Interfaces.Engine->GetViewAngles(); }
        }
    }
    //VM Offset Shit

    Vec3 vForward = {}, vRight = {}, vUp = {};
    Math::AngleVectors(eyeAngles, &vForward, &vRight, &vUp);

    // viewmodel offsets should probably be locked between 2 values, just like we should clamp our eye angles, however, I don't want to, f*ck you

    Vec3 vEyePosition = eyePosition + (
        (vRight * Vars::Visuals::VMOffX.m_Var) +
        (vForward * Vars::Visuals::VMOffY.m_Var) +
        (vUp * Vars::Visuals::VMOffZ.m_Var)
        );

    eyeAngles.z += Vars::Visuals::VMRoll.m_Var; //VM Roll

    originalFn(ecx, edx, owner, vEyePosition, eyeAngles);
}

void Viewmodel::Init() {
	auto Src = reinterpret_cast<void*>(g_Pattern.Find(
		L"client.dll", L"55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57"));
	Func.Hook(Src, Hook);
}