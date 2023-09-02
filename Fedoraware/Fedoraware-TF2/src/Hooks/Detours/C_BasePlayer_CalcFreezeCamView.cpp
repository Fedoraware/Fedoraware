#include "../Hooks.h"

MAKE_HOOK(C_BasePlayer_CalcFreezeCamView, g_Pattern.Find(L"client.dll", L"53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 57 8B F9 8B 07"), void, __fastcall,
		  void* ecx, void* edx, Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
{
	Hook.Original<FN>()(ecx, edx, eyeOrigin, eyeAngles, fov);
}