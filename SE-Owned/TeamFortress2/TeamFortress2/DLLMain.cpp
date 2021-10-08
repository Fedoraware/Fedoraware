#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/ChatInfo/ChatInfo.h"
#include "Features/Visuals/Visuals.h"
#include "Features/Vars.h"

#include "Features/Menu/Menu.h"
#include "Features/What/What.h"

#include "Features/Menu/InputHelper/InputHelper.h"
#include "Features/Menu/ConfigManager/ConfigManager.h"
#include "Features/Menu/../AttributeChanger/AttributeChanger.h"

#include "Features/PlayerList/PlayerList.h"

#include "SDK/Includes/Enums.h"

#include "Utils/Events/Events.h"

#include "SDK/Discord/include/discord_rpc.h"

int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

inline void SetupDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize("889495873183154226", &handlers, 0, "");
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")) || !WinAPI::GetModuleHandleW(_(L"ntdll.dll"))) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_dwDirectXDevice = **reinterpret_cast<DWORD**>(g_Pattern.Find(L"shaderapidx9.dll", L"A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);
	g_Hooks.Init();
	g_ConVars.Init();
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
		{ 0x0, _("Arial"), 16, 0, FONTFLAG_OUTLINE },

		//FONT_FEATURE
		{ 0x0, _("Verdana"), 16, 800, FONTFLAG_OUTLINE }
		});

	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "achievement_earned"});

	SetupDiscord();
	Discord_ClearPresence();

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Fedoraware Loaded!\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 255, 255 }, _("Credits: "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 127, 0, 255 }, _("M-FeD, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 72, 52, 225, 255 }, _("JAGNEmk, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 0, 234, 255, 255 }, _("Lak3, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 143, 5, 188, 255 }, _("spook593, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 225, 46, 48, 255 }, _("oxyGodmode, "));
	g_Interfaces.CVars->ConsoleColorPrintf({ 185, 255, 94, 255}, _("r00t\n"));
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Based on SE-Owned Public Source!\n"));
	g_Interfaces.Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_attached14.mp3");

	std::wstring defaultConfig = L"default";
	if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + defaultConfig)) {

		std::wstring s;
		StringToWString(s, "default");
		g_CFG.Load(s.c_str());
	}

	while (!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(420));

	g_Interfaces.Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_hacked02.mp3");
	g_GlobalInfo.unloadWndProcHook = true;
	g_Menu.m_bOpen = false;
	g_What.menuOpen = false;
	Vars::Visuals::SkyboxChanger.m_Var = false;
	Vars::Visuals::ThirdPerson.m_Var = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	g_Events.Destroy();
	g_Hooks.Release();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("Fedoraware Unloaded!\n"));

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
