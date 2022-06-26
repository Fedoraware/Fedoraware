#include "../Hooks.h"

//#pragma optimize( "", off )
//
//MAKE_HOOK(disconnect, g_Pattern.Find(L"engine.dll", L"8B 0D ? ? ? ? 85 C9 74 0E 8B 01 8B 80 ? ? ? ?"), void, __cdecl,
//		  CCommand& args)
//{
//	if (args.ArgC() == 2)
//	{
//		G::DisconnectReason = args[1];
//		Hook.Original<FN>()(args);
//	}
//	else
//	{
//
//		G::DisconnectReason = "";
//		Hook.Original<FN>()(args);
//	}
//}
//
//#pragma optimize( "", on ) 