#include "../Hooks.h"

MAKE_HOOK(CMatchInviteNotification_OnTick, S::CMatchInviteNotification_OnTick(), void, __fastcall, void* ecx, void* edx)
{
	static auto CMatchInviteNotification_AcceptMatch = reinterpret_cast<void(__thiscall*)(void*)>(S::CMatchInviteNotification_AcceptMatch());

	if (Vars::Misc::InstantAccept.Value == 2)
	{
		*reinterpret_cast<double*>(reinterpret_cast<DWORD>(ecx) + 424) = 0.0;
		// v3 = *(double*)(this + 424);
		//if ( v3 != 0.0 && Plat_FloatTime(v12) > *(double *)(this + 424) )
	}

	Hook.Original<FN>()(ecx, edx);

	if (Vars::Misc::InstantAccept.Value == 1)
	{
		CMatchInviteNotification_AcceptMatch(ecx);
	}
}