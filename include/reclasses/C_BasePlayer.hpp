#pragma once

#include "reclasses.hpp"

namespace re
{

class C_BasePlayer : public C_BaseEntity
{
public:
    inline static CTFPlayerShared &shared_(IClientEntity *self)
    {
        return *((CTFPlayerShared *) (unsigned(self) + 6092));
    }
    inline static Vector GetEyePosition(IClientEntity *self)
    {
        typedef Vector (*fn_t)(IClientEntity *);
        return vfunc<fn_t>(self, offsets::PlatformOffset(194, offsets::undefined, 194), 0)(self);
    }
    inline static Vector &GetEyeAngles(IClientEntity *self)
    {
        typedef Vector &(*fn_t)(IClientEntity *);
        return vfunc<fn_t>(self, offsets::PlatformOffset(195, offsets::undefined, 195), 0)(self);
    }
    inline static Vector &GetLocalEyeAngles(IClientEntity *self)
    {
        typedef Vector &(*fn_t)(IClientEntity *);
        return vfunc<fn_t>(self, offsets::PlatformOffset(196, offsets::undefined, 196), 0)(self);
    }
    inline static IClientEntity *GetEquippedDemoShield(IClientEntity *self)
    {
        static auto signature = e8call_direct(gSignatures.GetClientSignature("E8 ? ? ? ? 8B 55 ? 85 C0 0F 84 ? ? ? ? 89 5C 24"));
        typedef IClientEntity *(*GetEquippedDemoShield_t)(IClientEntity *);
        static GetEquippedDemoShield_t GetEquippedDemoShield_fn = GetEquippedDemoShield_t(signature);
        return GetEquippedDemoShield_fn(self);
    }
};
} // namespace re
