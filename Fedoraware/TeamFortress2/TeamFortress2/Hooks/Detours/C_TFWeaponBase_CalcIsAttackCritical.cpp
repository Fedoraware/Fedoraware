#include "../Hooks.h"

#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Commands/Commands.h"

bool CritHandler()
{
    if (!I::Prediction->m_bFirstTimePredicted)
    {
        return false;
    }

    const auto& pLocal = g_EntityCache.GetLocal();
    const auto& pWeapon = g_EntityCache.GetWeapon();

    if (!pWeapon || pLocal)
    {
        return false;
    }

    {
        static int s_nPreviousTickcount = 0;

        if (s_nPreviousTickcount == I::GlobalVars->tickcount)
        {
            return false;
        }

        s_nPreviousTickcount = I::GlobalVars->tickcount;
    }

    {
        if (pWeapon->GetWeaponID() == TF_WEAPON_MINIGUN ||
            pWeapon->GetWeaponID() == TF_WEAPON_FLAMETHROWER)
        {
            auto nPreviousAmmoCount = pLocal->GetAmmoCount(pWeapon->m_iPrimaryAmmoType());
            static auto nNewAmmoCount = nPreviousAmmoCount;

            const auto bHasFiredBullet = nNewAmmoCount != nPreviousAmmoCount;

            if (!bHasFiredBullet)
            {
                return false;
            }
        }
    }

    return true;
}

MAKE_HOOK(C_TFWeaponBase_CalcIsAttackCritical, g_Pattern.Find(L"client.dll", L"53 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 14 85 DB 0F 84"), void, __fastcall,
          void* ecx, void* edx)
{
    const auto& pLocal = g_EntityCache.GetLocal();
    const auto& pWeapon = g_EntityCache.GetWeapon();
    if (!pLocal || !pWeapon)
    {
        return Hook.Original<FN>()(ecx, edx);
    }

    if (CritHandler()) //  this completely fucks the crit bucket for me
    {
        return;
    }

    const auto nPreviousWeaponMode = pWeapon->m_iWeaponMode();
    pWeapon->m_iWeaponMode() = 0;

    Hook.Original<FN>()(ecx, edx);
    
    pWeapon->m_iWeaponMode() = nPreviousWeaponMode;
}

//MAKE_HOOK(C_TFWeaponBase_CalcIsAttackCriticalHelper, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC"), bool, __fastcall,
//		  void* ecx, void* edx)
//{
//    if (!g_EntityCache.GetLocal() || !g_EntityCache.GetWeapon())
//    {
//        return Hook.Original<FN>()(ecx, edx);
//    }
//
//    if (!CritHandler())
//    {
//        return;
//    }
//    //static bool bInitCmd = false;
//    //static std::map<void*, bool> returnAddresses;
//    //if (!bInitCmd)
//    //{
//    //    F::Commands.Register("crit_print_3", [](const std::deque<std::string>& args)
//    //                         {
//    //                             for (auto& retaddr : returnAddresses)
//    //                             {
//    //                                 I::Cvar->ConsolePrintf("%x\n", retaddr.first);
//    //                             }
//    //                         });
//
//    //    bInitCmd = true;
//    //}
//
//    //auto retaddr = _ReturnAddress();
//    //if (returnAddresses.find(retaddr) == returnAddresses.end())
//    //{
//    //    returnAddresses[retaddr] = true;
//    //}
//
//	return Hook.Original<FN>()(ecx, edx);
//}
//
