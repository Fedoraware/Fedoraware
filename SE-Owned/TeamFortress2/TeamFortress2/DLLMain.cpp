#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/ChatInfo/ChatInfo.h"
#include "Features/Visuals/Visuals.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")))
		std::this_thread::sleep_for(std::chrono::seconds(5));
	
	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_Hooks.Init();
	g_ConVars.Init();
	g_ChatInfo.AddListeners();
	g_Draw.InitFonts
	({
		//FONT_ESP
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE },
		//FONT_ESP_OUTLINED
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_NAME
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_NONE },
		//FONT_ESP_NAME_OUTLINED
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_COND
		{ 0x0, _("Consolas"), 10, 0, FONTFLAG_NONE },
		//FONT_ESP_COND_OUTLINED
		{ 0x0, _("Consolas"), 10, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_PICKUPS
		{ 0x0, _("Consolas"), 13, 0, FONTFLAG_NONE },
		//FONT_ESP_PICKUPS_OUTLINED
		{ 0x0, _("Consolas"), 13, 0, FONTFLAG_OUTLINE },

		//FONT_MENU
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_ANTIALIAS },

		//FONT_DEBUG
		{ 0x0, _("Arial"), 16, 0, FONTFLAG_OUTLINE }
	});

	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 0, 255 }, _("SE-Owned Loaded!\n"));

	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(420));

	g_ChatInfo.RemoveListeners();
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("SE-Owned Unloaded!\n"));
	g_Hooks.Release();
	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it

	WinAPI::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		Utils::RemovePEH(hinstDLL);

		if (auto hMainThread = WinAPI::CreateThread(0, 0, MainThread, hinstDLL, 0, 0))
			WinAPI::CloseHandle(hMainThread);
	}

	return TRUE;
}