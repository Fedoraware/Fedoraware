#include "NoSpread.h"
#define VALVE_RAND_MAX 0x7FFF


void CNoSpread::CreateMoveProjectile(CUserCmd* cmd) // Credits to Nullworks/Cathook team :))
{
    const auto pLocal = g_EntityCache.GetLocal();
    const auto pWeapon = g_EntityCache.GetWeapon();
    if (!pLocal || !pWeapon)
        return;

    if (!pLocal->IsAlive())
        return;

    if (!Vars::NoSpread::Projectile.Value)
        return;

    // Credits to https://www.unknowncheats.me/forum/team-fortress-2-a/139094-projectile-nospread.html
 
    // Projectile check
    if (G::CurWeaponType != EWeaponType::PROJECTILE)
        return;

    // Set up Random Seed
    int cmd_num = cmd->command_number;
    Utils::RandomSeed(MD5_PseudoRandom(cmd_num) & 0x7FFFFFFF);
    Utils::SharedRandomInt(MD5_PseudoRandom(cmd_num) & 0x7FFFFFFF, "SelectWeightedSequence", 0, 0, 0);
    for (int i = 0; i < 6; ++i)
       Utils::RandomFloat(-1, 1);

    // Beggars check
    if (pWeapon->GetItemDefIndex() == Soldier_m_TheBeggarsBazooka)
    {
        // Player has 0 loaded rockets and reload mode is not 2 (reloading and ready to release)
        bool no_loaded_rockets = pWeapon->GetClip1() == 0 && pWeapon->GetReloadMode() != 2;
        // Player is attacking and reload is not 0 (not reloading)
        bool loading_rockets = cmd->buttons & IN_ATTACK && pWeapon->GetReloadMode() != 0;
        if (no_loaded_rockets || loading_rockets)
            return;
    }
    // Huntsman check
    else if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW)
    {
        if (cmd->buttons & IN_ATTACK || pWeapon->GetChargeBeginTime() == 0)
            return;
    }
    // Rest of weapons
    else if (!(cmd->buttons & IN_ATTACK))
        return;

    switch (pWeapon->GetWeaponID())
    {
    case TF_WEAPON_SYRINGEGUN_MEDIC:
    {
        float spread = 1.5f;
        cmd->viewangles.x -= Utils::RandomFloat(-spread, spread);
        cmd->viewangles.y -= Utils::RandomFloat(-spread, spread);
        Math::ClampAngles(cmd->viewangles);
        G::SilentTime = true;
        break;
    }
    case TF_WEAPON_COMPOUND_BOW:
    {
        Vec3 view = I::EngineClient->GetViewAngles();
        Vec3 spread;
        Vec3 src;

        pWeapon->GetProjectileFireSetup(pLocal, Vec3(23.5f, 8.0f, -3.0f), &src, &spread, false, 2000.0f);
        float flTotalChargeTime = I::GlobalVars->curtime - pWeapon->GetChargeBeginTime();
        if (flTotalChargeTime >= 5.0f)
        {
            float frand = (float)Utils::RandomInt() / VALVE_RAND_MAX;
            spread.x += -6 + frand * 12.f;
            frand = (float)Utils::RandomInt() / VALVE_RAND_MAX;
            spread.y += -6 + frand * 12.f;
        }
        spread -= view;
        cmd->viewangles -= spread;
        Math::ClampAngles(cmd->viewangles);
        G::SilentTime = true;
        break;
    }
    default:
        Vec3 view = I::EngineClient->GetViewAngles();
        Vec3 spread;

        pWeapon->GetSpreadAngles(spread);
        spread -= view;
        cmd->viewangles -= spread;
        Math::ClampAngles(cmd->viewangles);
        G::SilentTime = true;
        break;
    }
}
