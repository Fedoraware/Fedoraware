//#include "../Hooks.h"
//
//MAKE_HOOK(OpenVoiceMenu, g_Pattern.Find(L"client.dll", L"55 8B EC 83 EC 64 56 E8"), void, __cdecl,
//		  int index)
//{
//	if (I::TFGCClientSystem)
//	{
//		*((bool*)I::TFGCClientSystem + 1440) = false;
//	}
//	Hook.Original<FN>()(index);
//}