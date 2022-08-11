#include "../Hooks.h"
#include "../../Features/CritHack/CritHack.h"

MAKE_HOOK(C_BaseCombatWeapon_AddToCritBucket, g_Pattern.Find(L"client.dll", L"55 8B EC A1 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 48 ? 0F 2F C8 76 1D F3 0F 58 45 ? 0F 2F C8 F3 0F 11 81 ? ? ? ? 77 03 0F 28 C1 F3 0F 11 81 ? ? ? ? 5D C2 04 00"), void, __fastcall,
	void* ecx, void* edx, float flAmount)
{
    ///*static bool bInitCmd = false;
    //static std::map<void*, bool> returnAddresses;
    //if (!bInitCmd)
    //{
    //    F::Commands.Register("crit_print_2", [](const std::deque<std::string>& args)
    //                         {
    //                             for (auto& retaddr : returnAddresses)
    //                             {
    //                                 I::Cvar->ConsolePrintf("%x\n", retaddr.first);
    //                             }
    //                         });

    //    bInitCmd = true;
    //}

    //auto retaddr = _ReturnAddress();
    //if (returnAddresses.find(retaddr) == returnAddresses.end())
    //{
    //    returnAddresses[retaddr] = true;
    //}*/

    //float m_flCritTokenBucket = ecx[661];
    //float flCap = I::Cvar->FindVar("tf_weapon_criticals_bucket_cap")->GetFloat();
    //if (flCap > m_flCritTokenBucket)
    //{
    //    float v4 = m_flCritTokenBucket + flAmount;
    //    ecx[661] = v4;
    //    //I::Cvar->ConsolePrintf("[C_BaseCombatWeapon::AddToCritBucket] m_flCritTokenBucket = %f\n", v4);
    //    if (flCap <= v4)
    //        v4 = flCap;
    //    ecx[661] = v4;
    //   /* I::Cvar->ConsolePrintf("[C_BaseCombatWeapon::AddToCritBucket] Min( m_flCritTokenBucket, flCap ) %f\n", v4);*/
    //}

    if (F::CritHack.ProtectData)
    {
        return;
    }

	return Hook.Original<FN>()(ecx, edx, flAmount);
}