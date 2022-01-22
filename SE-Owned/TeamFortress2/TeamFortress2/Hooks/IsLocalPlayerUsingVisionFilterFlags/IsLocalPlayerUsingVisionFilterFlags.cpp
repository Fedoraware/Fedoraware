#include "IsLocalPlayerUsingVisionFilterFlags.h"

#define TF_VISION_FILTER_NONE			0
#define TF_VISION_FILTER_PYRO			(1<<0)		// 1
#define TF_VISION_FILTER_HALLOWEEN		(1<<1)		// 2
#define TF_VISION_FILTER_ROME			(1<<2)		// 4

bool __cdecl IsLocalPlayerUsingVisionFilterFlags::Func(int nFlags, bool bWeaponsCheck)
{
	bool ret = Hook.Original<fn>()(nFlags, bWeaponsCheck);
	//g_Interfaces.CVars->ConsolePrintf("Ret value: %d\nnFlags: %d\nbWeaponsCheck: %d", ret, nFlags, bWeaponsCheck);
	switch (Vars::Visuals::Vision.m_Var) {
	case 1:
		if (nFlags == 1) {
			return true;
		}
		else {
			return false;
		}
	case 2:
		if (nFlags == 2) {
			return true;
		}
		else {
			return false;
		}
	case 3:
		if (nFlags == 4) {
			return true;
		}
		else {
			return false;
		}
	}

	return ret;
}

void IsLocalPlayerUsingVisionFilterFlags::Init()
{
	//[actual address in first opcode] E8 ? ? ? ? 0F B6 4D FF 83 C4 08 
	//fn FN = reinterpret_cast<fn>(g_Pattern.Find(L"client.dll", L"55 8B EC 57 8B F9 8B 0D ? ? ? ? 8B 01 8B 40 20"));
	DWORD IsLocalPlayerUsingVisionFilterFlagsAddress = g_Pattern.Find(L"client.dll", L"E8 ? ? ? ? 0F B6 4D FF 83 C4 08") + 1;
	auto IsLocalPlayerUsingVisionFilterFlagsHook = reinterpret_cast<fn>((*reinterpret_cast<PDWORD>(IsLocalPlayerUsingVisionFilterFlagsAddress) + IsLocalPlayerUsingVisionFilterFlagsAddress + 0x4));
	Hook.Hook(IsLocalPlayerUsingVisionFilterFlagsHook, Func);
}
