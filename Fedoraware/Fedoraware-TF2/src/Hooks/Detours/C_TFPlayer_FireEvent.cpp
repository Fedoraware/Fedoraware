#include "../Hooks.h"

namespace S
{
	MAKE_SIGNATURE(SpawnHalloweenSpellFootsteps, CLIENT_DLL, "55 8B EC A1 ? ? ? ? 83 EC ? 53 8B 5D", 0x0);
}

//Credits to mfed for creating this entire thing
int ColorToInt(const Color_t& col)
{
    return col.r << 16 | col.g << 8 | col.b;
}

MAKE_HOOK(C_TFPlayer_FireEvent, S::C_TFPlayer_FireEvent(), void, __fastcall,
    CBaseEntity* ecx, void* edx, const Vector& origin, const QAngle& angles, int event_, const char* options)
{
    static auto fnSpawnHalloweenSpellFootsteps = S::SpawnHalloweenSpellFootsteps.As<void* (__thiscall*)(CBaseEntity*, ParticleAttachment_t, int)>();

    if (!Vars::Visuals::HalloweenSpellFootsteps.Value || event_ != 7001 || ecx != g_EntityCache.GetLocal())
    {
        return Hook.Original<FN>()(ecx, edx, origin, angles, event_, options);
    }

    const auto feetcolor = ColorToInt(Vars::Visuals::ColorType.Value == 0 ? Colors::FeetColor : Utils::Rainbow());
	fnSpawnHalloweenSpellFootsteps(g_EntityCache.GetLocal(), PATTACH_ABSORIGIN, Vars::Visuals::ColorType.Value == 1 ? 2 : feetcolor);

    return Hook.Original<FN>()(ecx, edx, origin, angles, event_, options);
}