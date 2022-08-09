#pragma once

#include "common.hpp"

struct WeaponData_t
{
    int m_nDamage;
    int m_nBulletsPerShot;
    float m_flRange;
    float m_flSpread;
    float m_flPunchAngle;
    float m_flTimeFireDelay;   // Time to delay between firing
    float m_flTimeIdle;        // Time to idle after firing
    float m_flTimeIdleEmpty;   // Time to idle after firing last bullet in clip
    float m_flTimeReloadStart; // Time to start into a reload (ie. shotgun)
    float m_flTimeReload;      // Time to reload
    bool m_bDrawCrosshair;     // Should the weapon draw a crosshair
    int m_iProjectile;         // The type of projectile this mode fires
    int m_iAmmoPerShot;        // How much ammo each shot consumes
    float m_flProjectileSpeed; // Start speed for projectiles (nail, etc.); NOTE: union with something non-projectile
    float m_flSmackDelay;      // how long after swing should damage happen for melee weapons
    bool m_bUseRapidFireCrits;
};

class weapon_info
{
public:
    float crit_bucket{};
    unsigned int weapon_seed{};
    unsigned unknown1{};
    unsigned unknown2{};
    bool unknown3{};
    float m_flCritTime{};
    int crit_attempts{};
    int crit_count{};
    float observed_crit_chance{};
    bool unknown7{};
    int weapon_mode{};
    int weapon_data{};
    weapon_info()
    {
    }
    void Load(IClientEntity *weapon)
    {
        crit_bucket          = *(float *) ((uintptr_t) weapon + 0xa3c);
        weapon_seed          = *(unsigned int *) ((uintptr_t) weapon + 0xb40);
        unknown1             = *(unsigned int *) ((uintptr_t) weapon + 0xb34);
        unknown2             = *(unsigned int *) ((uintptr_t) weapon + 0xb38);
        unknown3             = *(bool *) ((uintptr_t) weapon + 0xb1b);
        m_flCritTime         = *(float *) ((uintptr_t) weapon + 0xb44);
        crit_attempts        = *(int *) ((uintptr_t) weapon + 0xa40);
        crit_count           = *(int *) ((uintptr_t) weapon + 0xa44);
        observed_crit_chance = *(float *) ((uintptr_t) weapon + 0xc00);
        unknown7             = *(bool *) ((uintptr_t) weapon + 0xb1c);
        // No need to restore
        weapon_mode = *(int *) ((uintptr_t) weapon + 0xb08);
        weapon_data = *(int *) ((uintptr_t) weapon + 0xb14);
    }
    weapon_info(IClientEntity *weapon)
    {
        Load(weapon);
    }
    void restore_data(IClientEntity *weapon)
    {
        *(float *) ((uintptr_t) weapon + 0xa3c)        = crit_bucket;
        *(unsigned int *) ((uintptr_t) weapon + 0xb40) = weapon_seed;
        *(unsigned int *) ((uintptr_t) weapon + 0xb34) = unknown1;
        *(unsigned int *) ((uintptr_t) weapon + 0xb38) = unknown2;
        *(bool *) ((uintptr_t) weapon + 0xb1b)         = unknown3;
        *(float *) ((uintptr_t) weapon + 0xb44)        = m_flCritTime;
        *(int *) ((uintptr_t) weapon + 0xa40)          = crit_attempts;
        *(int *) ((uintptr_t) weapon + 0xa44)          = crit_count;
        *(float *) ((uintptr_t) weapon + 0xc00)        = observed_crit_chance;
        *(bool *) ((uintptr_t) weapon + 0xb1c)         = unknown7;
    }
    bool operator==(const weapon_info &B) const
    {
        return crit_bucket == B.crit_bucket && weapon_seed == B.weapon_seed && unknown1 == B.unknown1 && unknown2 == B.unknown2 && unknown3 == B.unknown3 && m_flCritTime == B.m_flCritTime && crit_attempts == B.crit_attempts && crit_count == B.crit_count && observed_crit_chance == B.observed_crit_chance && unknown7 == B.unknown7;
    }
    bool operator!=(const weapon_info &B) const
    {
        return !(*this == B);
    }
};

inline WeaponData_t *GetWeaponData(IClientEntity *weapon)
{
    weapon_info info(weapon);
    int WeaponData = info.weapon_data;
    int WeaponMode = info.weapon_mode;
    return (WeaponData_t *) (WeaponData + sizeof(WeaponData_t) * WeaponMode + 1784);
}
