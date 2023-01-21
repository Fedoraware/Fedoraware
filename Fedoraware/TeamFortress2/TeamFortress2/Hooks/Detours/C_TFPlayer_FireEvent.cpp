#include "../Hooks.h"

//Credits to mfed for creating this entire thing
int ColorToInt(const Color_t& col)
{
    return col.r << 16 | col.g << 8 | col.b;
}

MAKE_HOOK(C_TFPlayer_FireEvent, g_Pattern.Find(L"client.dll", L"55 8B EC 8B 45 ? 81 EC ? ? ? ? 53 56 57 8B F9 3D"), void, __fastcall,
    CBaseEntity* ecx, void* edx, const Vector& origin, const QAngle& angles, int event_, const char* options)
{
    static auto SpawnHalloweenSpellFootsteps = reinterpret_cast<void* (__thiscall*)(CBaseEntity*, ParticleAttachment_t, int)>(g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? 83 EC ? 53 8B 5D"));

    if (!Vars::Visuals::HalloweenSpellFootsteps.Value || event_ != 7001 || ecx != g_EntityCache.GetLocal())
    {
        return Hook.Original<FN>()(ecx, edx, origin, angles, event_, options);
    }

    auto feetcolor = ColorToInt(Vars::Visuals::ColorType.Value == 0 ? Colors::FeetColor : Utils::Rainbow());

    if (Vars::Visuals::DashOnly.Value && G::Teleporting == true) //this is better than just showing when you have the key down, i guess
    {
        SpawnHalloweenSpellFootsteps(g_EntityCache.GetLocal(), PATTACH_ABSORIGIN, Vars::Visuals::ColorType.Value == 1 ? 2 : feetcolor);
    }
    else {
        SpawnHalloweenSpellFootsteps(g_EntityCache.GetLocal(), PATTACH_ABSORIGIN, Vars::Visuals::ColorType.Value == 1 ? 2 : feetcolor);
    }

    return Hook.Original<FN>()(ecx, edx, origin, angles, event_, options);
}