#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/ChatInfo/ChatInfo.h"
#include "Features/Visuals/Visuals.h"
#include "Features/Vars.h"

#include "Features/Menu/Menu.h"

#include "Features/Menu/InputHelper/InputHelper.h"
#include "Features/Menu/ConfigManager/ConfigManager.h"
#include "Features/Menu/../AttributeChanger/AttributeChanger.h"

#include "SDK/Includes/Enums.h"

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

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
	g_Visuals.Init();
	g_Hooks.Init();
	g_ConVars.Init();
	g_ChatInfo.AddListeners();
	g_Draw.InitFonts
	({
		//FONT_ESP
		{ 0x0, _("Tahoma"), 12, 800, FONTFLAG_ANTIALIAS },
		//{ 0x0, _(Vars::Fonts::ESP_FONT), 12, 0, FONTFLAG_NONE },
		//FONT_ESP_OUTLINED
		{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },
		//{ 0x0, _(Vars::Fonts::ESP_FONT), 12, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_NAME
		{ 0x0, _("Tahoma"), 14, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_NAME_OUTLINED
		{ 0x0, _("Verdana"), 14, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_COND
		{ 0x0, _("Tahoma"), 10, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_COND_OUTLINED
		{ 0x0, _("Verdana"), 10, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_PICKUPS
		{ 0x0, _("Tahoma"), 13, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_PICKUPS_OUTLINED
		{ 0x0, _("Verdana"), 13, 0, FONTFLAG_OUTLINE },

		//FONT_MENU
		{ 0x0, _("DejaVu Sans"), 16, 200, FONTFLAG_ANTIALIAS},

		//FONT_DEBUG
		{ 0x0, _("Arial"), 16, 0, FONTFLAG_OUTLINE }
		});

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Fedoraware Loaded!\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, _("Credits: "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 127, 0, 255 }, _("M-FeD, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 72, 52, 225, 255 }, _("JAGNEmk, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 234, 255, 255 }, _("Lak3, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 143, 5, 188, 255 }, _("Spook593, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 225, 46, 48, 255 }, _("OxyGodmode\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Based on SE-Owned Public Source!\n"));

	//ConVar* skybox_set = new ConVar("skybox_set", "mr_04");
	ConCommandBase* skybox_set = new ConVar("skybox_set", "mr_04", (int)EConVarFlags::FCVAR_NOTIFY);
	g_Interfaces.CVars->RegisterConCommand(skybox_set);
	ConVar_Register(0);

	std::wstring figgy = L"default";
	if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + figgy)) {

		std::wstring s;
		StringToWString(s, "default");
		g_CFG.Load(s.c_str());
	}

	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(420));
	Vars::Visuals::SkyboxChanger.m_Var = false;
	g_ChatInfo.RemoveListeners();
	g_Menu.m_bOpen = false;
	Vars::Visuals::ThirdPerson.m_Var = false;
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("Fedoraware Unloaded!\n"));
	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
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