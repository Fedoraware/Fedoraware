#include "../Hooks.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Commands/Commands.h"

MAKE_HOOK(C_BaseCombatWeapon_IsAllowedToWithdrawFromCritBucket, g_Pattern.Find(L"client.dll", L"55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15"), bool, __fastcall,
	void* ecx, void* edx, float flDamage)
{


    //static auto C_BaseCombatWeapon_GetWpnData = reinterpret_cast<void*** (__cdecl*)(unsigned __int16 a1)>(g_Pattern.Find(L"client.dll", L"55 8B EC 66 8B 45 08 66 3B 05 ? ? ? ? 73 1A B9 ? ? ? ? 66 3B C1 74 10 0F B7 C8 A1 ? ? ? ? 03 C9 8B 44 C8 0C 5D C3"));

    //static bool bInitCmd = false;
    //static std::map<void*, bool> returnAddresses;
    //if (!bInitCmd)
    //{
    //    F::Commands.Register("crit_print_1", [](const std::deque<std::string>& args)
    //                         {
    //                             for (auto &retaddr : returnAddresses)
    //                             {
    //                                 I::Cvar->ConsolePrintf("%x\n", retaddr.first);
    //                            }
    //                         });

    //    bInitCmd = true;
    //}

    //auto retaddr = _ReturnAddress();
    //if (returnAddresses.find(retaddr) == returnAddresses.end())
    //{
    //    returnAddresses[retaddr] = true;
    //}

    //unsigned __int16 v3; // ax
    //float v4; // xmm2_4
    //float m_flCritTokenBucket; // xmm1_4
    //float flCost; // xmm0_4
    //float v8; // xmm1_4
    //float flBottom; // xmm0_4

    //v3 = *(WORD*)(ecx + 2778);
    //++* (DWORD*)(ecx + 2652);
    //if (*((BYTE*)C_BaseCombatWeapon_GetWpnData(v3) + 1736) )
    //    v4 = 0.5;
    //else
    //    v4 = (float)(fminf(
    //    fmaxf(
    //    (float)((float)((float)*(int*)(ecx + 2652) / (float)*(int*)(ecx + 2648)) - 0.1) * 1.1111112,
    //    0.0),
    //    1.0)
    //    * 2.0)
    //    + 1.0;
    //m_flCritTokenBucket = *(float*)(ecx + 2644);
    //flCost = (float)(flDamage * 3.0) * v4;
    //if (flCost > m_flCritTokenBucket)
    //    return 0;
    //v8 = m_flCritTokenBucket - flCost;
    //*(float*)(ecx + 2644) = v8;
    ////I::Cvar->ConsolePrintf("[C_BaseCombatWeapon::IsAllowedToWithdrawFromCritBucket] RemoveFromCritBucket(%f)\n", v8);
    //flBottom = I::Cvar->FindVar("tf_weapon_criticals_bucket_bottom")->GetFloat();
    //if (flBottom > v8)
    //{
    //    *(float*)(ecx + 2644) = flBottom;
    //    //I::Cvar->ConsolePrintf("[C_BaseCombatWeapon::IsAllowedToWithdrawFromCritBucket] Bucket less than minimum: set to %f\n", v8);
    //}


   
    if (F::CritHack.ProtectData)
    {
        return true;
    }

    return Hook.Original<FN>()(ecx, edx, flDamage);
}