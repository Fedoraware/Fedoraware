#include "Hooks/HookManager.h"
#include "Hooks/PatchManager/PatchManager.h"

#include "Features/NetVarHooks/NetVarHk.h"
#include "Features/Visuals/Visuals.h"
#include "Features/Misc/Misc.h"
#include "Features/Vars.h"
#include "Features/TickHandler/TickHandler.h"

#include "Features/Menu/Menu.h"

#include "Features/Menu/ConfigManager/ConfigManager.h"
#include "Features/Menu/../AttributeChanger/AttributeChanger.h"
#include "Features/Commands/Commands.h"

#include "SDK/Includes/Enums.h"
#include "Utils/Events/Events.h"

#include "SDK/Discord/include/discord_rpc.h"
#include "Features/Discord/Discord.h"
#include "Utils/Minidump/Minidump.h"

#include "Core/Core.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!GetModuleHandleW(L"mss32.dll") ||
		   !GetModuleHandleW(L"ntdll.dll") ||
		   !GetModuleHandleW(L"stdshader_dx9.dll") ||
		   !GetModuleHandleW(L"materialsystem.dll"))
	{
		Sleep(2000);
	}

	g_Core.Load();

	while (!GetAsyncKeyState(VK_F11) || F::Menu.IsOpen)
	{
		Sleep(20);
	}

	g_Core.Unload();

	FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		#ifndef _DEBUG
		SetUnhandledExceptionFilter(Minidump::ExceptionFilter);
		#endif

		Utils::RemovePEH(hinstDLL);
		if (const auto hMainThread = CreateThread(nullptr, 0, MainThread, hinstDLL, 0, nullptr))
		{
			CloseHandle(hMainThread);
		}
	}

	return TRUE;
}
