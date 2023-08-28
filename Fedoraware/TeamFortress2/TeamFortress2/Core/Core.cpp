#include "Core.h"

#include "../Hooks/HookManager.h"
#include "../Hooks/PatchManager/PatchManager.h"

#include "../Features/NetVarHooks/NetVarHk.h"
#include "../Features/Visuals/Visuals.h"
#include "../Features/Misc/Misc.h"
#include "../Features/Vars.h"
#include "../Features/TickHandler/TickHandler.h"

#include "../Features/Menu/Menu.h"

#include "../Features/Menu/ConfigManager/ConfigManager.h"
#include "../Features/Menu/../AttributeChanger/AttributeChanger.h"
#include "../Features/Commands/Commands.h"

#include "../SDK/Includes/Enums.h"
#include "../Utils/Events/Events.h"

#include "../SDK/Discord/include/discord_rpc.h"
#include "../Features/Discord/Discord.h"
#include "../Utils/Minidump/Minidump.h"

void LoadDefaultConfig()
{
	// Load default visuals
	g_CFG.LoadVisual(g_CFG.GetCurrentVisuals());
	g_CFG.LoadConfig(g_CFG.GetCurrentConfig());

	Sleep(200);

	// TODO: Do this inside g_Draw (and use a struct?)
	g_Draw.RemakeFonts
	({
		{0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value},
		{0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value},
		{0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value},
		{0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value},
		{0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value},
		{0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value},
		{0x0, "Verdana", 18, 1600, FONTFLAG_ANTIALIAS},
		{0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW},
	});

	F::Menu.ConfigLoaded = true;
}

void CCore::OnLoaded()
{
	LoadDefaultConfig();

	I::Cvar->ConsoleColorPrintf(Vars::Menu::Colors::MenuAccent, "%s Loaded!\n", Vars::Menu::CheatName.c_str());
	I::EngineClient->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_attached14.mp3");

	// Check the DirectX version
	const int dxLevel = g_ConVars.FindVar("mat_dxlevel")->GetInt();
	if (dxLevel < 90)
	{
		MessageBoxA(nullptr, "Your DirectX version is too low!\nPlease use dxlevel 90 or higher", "dxlevel too low", MB_OK | MB_ICONWARNING);
	}
}

void CCore::Load()
{
	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();

	// Initialize hooks & memory stuff
	{
		g_HookManager.Init();
		g_PatchManager.Init();
		F::NetHooks.Init();
	}

	g_ConVars.Init();
	F::Ticks.Reset();

	F::Commands.Init();
	F::DiscordRPC.Init();

	g_Events.Setup({
		"vote_cast", "player_changeclass", "player_connect", "player_hurt", "achievement_earned", "player_death", "vote_started", "teamplay_round_start", "player_spawn", "item_pickup"
	}); // all events @ https://github.com/tf2cheater2013/gameevents.txt

	OnLoaded();
}

void CCore::Unload()
{
	I::EngineClient->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_hacked02.mp3");
	G::UnloadWndProcHook = true;
	Vars::Visuals::SkyboxChanger.Value = false;
	Vars::Visuals::ThirdPerson.Value = false;

	Sleep(100);

	g_Events.Destroy();
	g_HookManager.Release();
	g_PatchManager.Restore();

	F::DiscordRPC.Shutdown();

	Sleep(100);

	F::Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	I::Cvar->ConsoleColorPrintf(Vars::Menu::Colors::MenuAccent, "%s Unloaded!\n", Vars::Menu::CheatName.c_str());
}

bool CCore::ShouldUnload()
{
	const bool unloadKey = GetAsyncKeyState(VK_F11) & 0x8000;
	return unloadKey && !F::Menu.IsOpen;
}
