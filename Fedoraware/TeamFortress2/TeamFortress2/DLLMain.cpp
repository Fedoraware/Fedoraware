#include "Hooks/Hooks.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/ChatInfo/ChatInfo.h"
#include "Features/Visuals/Visuals.h"
#include "Features/Misc/Misc.h"
#include "Features/Vars.h"

#include "Features/Menu/Menu.h"

#include "Features/Menu/InputHelper/InputHelper.h"
#include "Features/Menu/ConfigManager/ConfigManager.h"
#include "Features/Menu/../AttributeChanger/AttributeChanger.h"

#include "SDK/Includes/Enums.h"

#include "Utils/Events/Events.h"

#include "SDK/Discord/include/discord_rpc.h"
#include "Features/Discord/Discord.h"

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


//Christ this shit is awful
DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")) || !WinAPI::GetModuleHandleW(_(L"ntdll.dll")) || !WinAPI::GetModuleHandleW(_(L"stdshader_dx9.dll"))) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_Hooks.Init();
	g_ConVars.Init();
	std::wstring defaultConfig = L"default";
	if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + defaultConfig)) {

		std::wstring s;
		StringToWString(s, "default");
		g_CFG.Load(s.c_str());
	}
	g_Draw.InitFonts
	({
		{ 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.m_Var, Vars::Fonts::FONT_ESP::nWeight.m_Var, Vars::Fonts::FONT_ESP::nFlags.m_Var},
		{ 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.m_Var, Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var, Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.m_Var, Vars::Fonts::FONT_ESP_COND::nWeight.m_Var, Vars::Fonts::FONT_ESP_COND::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.m_Var, Vars::Fonts::FONT_MENU::nWeight.m_Var, Vars::Fonts::FONT_MENU::nFlags.m_Var},
		{ 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.m_Var, Vars::Fonts::FONT_INDICATORS::nWeight.m_Var, Vars::Fonts::FONT_INDICATORS::nFlags.m_Var},
	});
	g_Menu.config = true;
	//g_Draw.InitFonts
	//({
		//FONT_ESP
		//{ 0x0, _("Tahoma"), 12, 800, FONTFLAG_ANTIALIAS },
		//{ 0x0, _(Vars::Fonts::ESP_FONT), 12, 0, FONTFLAG_NONE },
		//FONT_ESP_OUTLINED
		//{ 0x0, _("Verdana"), 12, 0, FONTFLAG_OUTLINE },
		//{ 0x0, _(Vars::Fonts::ESP_FONT), 12, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_NAME
		//{ 0x0, _("Tahoma"), 14, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_NAME_OUTLINED
		//{ 0x0, _("Verdana"), 14, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_COND
		//{ 0x0, _("Tahoma"), 10, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_COND_OUTLINED
		//{ 0x0, _("Verdana"), 10, 0, FONTFLAG_OUTLINE },

		//FONT_ESP_PICKUPS
		//{ 0x0, _("Tahoma"), 13, 800, FONTFLAG_ANTIALIAS },
		//FONT_ESP_PICKUPS_OUTLINED
		//{ 0x0, _("Verdana"), 13, 0, FONTFLAG_OUTLINE },

		//FONT_MENU
		//{ 0x0, _("DejaVu Sans"), 16, 200, FONTFLAG_ANTIALIAS},

		////FONT_DEBUG
		//{ 0x0, _("Arial"), 16, 0, FONTFLAG_OUTLINE },

		////FONT_INDICATORS
		//{ 0x0, _("Tahoma"), 13, 0, FONTFLAG_OUTLINE }
		//});

	SetupDiscord();
	Discord_ClearPresence();
	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "achievement_earned", "player_death", "vote_started"});
	// all events @ https://github.com/tf2cheater2013/gameevents.txt

	g_Interfaces.CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Fedoraware Loaded!\n"));
	g_Interfaces.Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_attached14.mp3");

	while (!GetAsyncKeyState(VK_F11) || g_Menu.m_bOpen) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		g_DiscordRPC.vFunc();
		g_Misc.SteamRPC();
	}

	g_Interfaces.Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_hacked02.mp3");
	g_GlobalInfo.unloadWndProcHook = true;
	g_Menu.m_bOpen = false;
	g_Menu.menuOpen = false;
	Vars::Visuals::SkyboxChanger.m_Var = false;
	Vars::Visuals::ThirdPerson.m_Var = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	g_Events.Destroy();
	g_Hooks.Release();
	Discord_ClearPresence();
	Discord_Shutdown();

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

#ifdef DEBUG
		WinAPI::CreateThread(0, 0, MainThread, hinstDLL, 0, 0);
#endif

		Utils::RemovePEH(hinstDLL);
		if (auto hMainThread = WinAPI::CreateThread(0, 0, MainThread, hinstDLL, 0, 0))
			WinAPI::CloseHandle(hMainThread);
	}

	return TRUE;
}
