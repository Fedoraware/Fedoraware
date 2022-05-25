#include "Hooks/HookManager.h"
#include "Hooks/PatchManager/PatchManager.h"

#include "Features/Glow/Glow.h"
#include "Features/Chams/Chams.h"
#include "Features/Chams/DMEChams.h"
#include "Features/Visuals/Visuals.h"
#include "Features/Camera/CameraWindow.h"
#include "Features/Misc/Misc.h"
#include "Features/Vars.h"

#include "Features/Menu/Menu.h"

#include "Features/Menu/ConfigManager/ConfigManager.h"
#include "Features/Menu/../AttributeChanger/AttributeChanger.h"

#include "SDK/Includes/Enums.h"
#include "Utils/Events/Events.h"

#include "SDK/Discord/include/discord_rpc.h"
#include "Features/Discord/Discord.h"

void Sleep(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int StringToWString(std::wstring& ws, const std::string& s)
{
	const std::wstring wsTmp(s.begin(), s.end());
	ws = wsTmp;

	return 0;
}

inline void SetupDiscord()
{
	DiscordEventHandlers handlers = {};
	Discord_Initialize("889495873183154226", &handlers, 0, "");
}

void InitRichPresence()
{
	SetupDiscord();
	Discord_ClearPresence();
}

void ShutdownRichPresence()
{
	Discord_ClearPresence();
	Discord_Shutdown();
}

void UpdateRichPresence()
{
	g_DiscordRPC.Update();
	g_Misc.SteamRPC();
}

void Loaded()
{
	I::CVars->ConsoleColorPrintf({ 255, 193, 75, 255 }, _("Fedoraware Loaded!\n"));
	I::Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_attached14.mp3");

	const int dxLevel = I::CVars->FindVar("mat_dxlevel")->GetInt();
	if (dxLevel < 90)
	{
		MessageBoxA(nullptr, _("Your DirectX version is too low!\nPlease use at dxlevel 90 or higher"), _("dxlevel too low"), MB_OK | MB_ICONWARNING);
	}
}

void Initialize()
{
	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();
	g_Glow.Init();
	g_Chams.Init();
	g_DMEChams.Init();
	g_CameraWindow.Init();
	g_HookManager.Init();
	g_ConVars.Init();
	g_PatchManager.Init();

	InitRichPresence();
}

void Uninitialize()
{
	I::Engine->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_hacked02.mp3");
	g_GlobalInfo.unloadWndProcHook = true;
	Vars::Visuals::SkyboxChanger.m_Var = false;
	Vars::Visuals::ThirdPerson.m_Var = false;

	Sleep(100);

	g_Events.Destroy();
	g_HookManager.Release();
	g_PatchManager.Restore();

	ShutdownRichPresence();

	Sleep(100);

	g_Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	I::CVars->ConsoleColorPrintf({ 255, 255, 0, 255 }, _("Fedoraware Unloaded!\n"));
}

void LoadDefaultConfig()
{
	if (std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + g_CFG.GetCurrentConfig())) {
		g_CFG.LoadConfig(g_CFG.GetCurrentConfig());
	}

	g_Draw.RemakeFonts
	({
		{ 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.m_Var, Vars::Fonts::FONT_ESP::nWeight.m_Var, Vars::Fonts::FONT_ESP::nFlags.m_Var},
		{ 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.m_Var, Vars::Fonts::FONT_ESP_NAME::nWeight.m_Var, Vars::Fonts::FONT_ESP_NAME::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.m_Var, Vars::Fonts::FONT_ESP_COND::nWeight.m_Var, Vars::Fonts::FONT_ESP_COND::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.m_Var, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.m_Var },
		{ 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.m_Var, Vars::Fonts::FONT_MENU::nWeight.m_Var, Vars::Fonts::FONT_MENU::nFlags.m_Var},
		{ 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.m_Var, Vars::Fonts::FONT_INDICATORS::nWeight.m_Var, Vars::Fonts::FONT_INDICATORS::nFlags.m_Var},
		{ 0x0, "Verdana", 18, 1600, FONTFLAG_ANTIALIAS},
		});
	g_Menu.ConfigLoaded = true;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!WinAPI::GetModuleHandleW(_(L"mss32.dll")) || !WinAPI::GetModuleHandleW(_(L"ntdll.dll")) || !WinAPI::GetModuleHandleW(_(L"stdshader_dx9.dll"))) {
		Sleep(5000);
	}

	Initialize();
	LoadDefaultConfig();

	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "achievement_earned", "player_death", "vote_started", "teamplay_round_start", "player_spawn", "item_pickup" }); // all events @ https://github.com/tf2cheater2013/gameevents.txt

	Loaded();

	while (!GetAsyncKeyState(VK_F11) || g_Menu.IsOpen) {
		Sleep(1000);
		UpdateRichPresence();
	}

	Uninitialize();

	WinAPI::FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{	
		Utils::RemovePEH(hinstDLL);
		if (const auto hMainThread = WinAPI::CreateThread(nullptr, 0, MainThread, hinstDLL, 0, nullptr))
		{
			WinAPI::CloseHandle(hMainThread);
		}
	}

	return TRUE;
}
