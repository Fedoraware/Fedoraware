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

#include "Features/LuaEngine/LuaEngine.h"
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

void Loaded()
{
	I::Cvar->ConsoleColorPrintf(Vars::Menu::Colors::MenuAccent, "%s Loaded!\n", Vars::Menu::CheatName.c_str());
	I::EngineClient->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_attached14.mp3");

	const int dxLevel = g_ConVars.FindVar("mat_dxlevel")->GetInt();
	if (dxLevel < 90)
	{
		MessageBoxA(nullptr, "Your DirectX version is too low!\nPlease use dxlevel 90 or higher", "dxlevel too low", MB_OK | MB_ICONWARNING);
	}
}

void Initialize()
{
	g_SteamInterfaces.Init();
	g_Interfaces.Init();
	g_NetVars.Init();

	g_Draw.RemakeFonts
	({
		{ 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value},
		{ 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value},
		{ 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value},
		{ 0x0, "Verdana", 18, 1600, FONTFLAG_ANTIALIAS},
		{ 0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW},
	 });

	// Initialize hooks & memory stuff
	{
		g_HookManager.Init();
		g_PatchManager.Init();
		F::NetHooks.Init();
	}

	g_ConVars.Init();
	F::LuaEngine.Init();
	F::Ticks.Reset();

	F::Statistics.m_SteamID = g_SteamInterfaces.User->GetSteamID();

	F::Commands.Init();

	InitRichPresence();
}

void Uninitialize()
{
	I::EngineClient->ClientCmd_Unrestricted("play vo/items/wheatley_sapper/wheatley_sapper_hacked02.mp3");
	G::UnloadWndProcHook = true;
	Vars::Visuals::SkyboxChanger.Value = false;
	Vars::Visuals::ThirdPerson.Value = false;

	Sleep(100);

	g_Events.Destroy();
	g_HookManager.Release();
	g_PatchManager.Restore();
	F::LuaEngine.Reset();

	ShutdownRichPresence();

	Sleep(100);

	F::Visuals.RestoreWorldModulation(); //needs to do this after hooks are released cuz UpdateWorldMod in FSN will override it
	I::Cvar->ConsoleColorPrintf(Vars::Menu::Colors::MenuAccent, "%s Unloaded!\n", Vars::Menu::CheatName.c_str());
}

void LoadDefaultConfig()
{
	if (std::filesystem::exists(g_CFG.GetConfigPath() + "\\Visuals\\default.fw"))
	{
		g_CFG.LoadVisual("default");
	}

	Sleep(200);

	if (std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + g_CFG.GetCurrentConfig() + ".fw"))
	{
		g_CFG.LoadConfig(g_CFG.GetCurrentConfig());
	}

	Sleep(200);

	g_Draw.RemakeFonts
	({
		{ 0x0, Vars::Fonts::FONT_ESP::szName.c_str(), Vars::Fonts::FONT_ESP::nTall.Value, Vars::Fonts::FONT_ESP::nWeight.Value, Vars::Fonts::FONT_ESP::nFlags.Value},
		{ 0x0, Vars::Fonts::FONT_ESP_NAME::szName.c_str(), Vars::Fonts::FONT_ESP_NAME::nTall.Value, Vars::Fonts::FONT_ESP_NAME::nWeight.Value, Vars::Fonts::FONT_ESP_NAME::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_ESP_COND::szName.c_str(), Vars::Fonts::FONT_ESP_COND::nTall.Value, Vars::Fonts::FONT_ESP_COND::nWeight.Value, Vars::Fonts::FONT_ESP_COND::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_ESP_PICKUPS::szName.c_str(), Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value, Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value, Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value },
		{ 0x0, Vars::Fonts::FONT_MENU::szName.c_str(), Vars::Fonts::FONT_MENU::nTall.Value, Vars::Fonts::FONT_MENU::nWeight.Value, Vars::Fonts::FONT_MENU::nFlags.Value},
		{ 0x0, Vars::Fonts::FONT_INDICATORS::szName.c_str(), Vars::Fonts::FONT_INDICATORS::nTall.Value, Vars::Fonts::FONT_INDICATORS::nWeight.Value, Vars::Fonts::FONT_INDICATORS::nFlags.Value},
		{ 0x0, "Verdana", 18, 1600, FONTFLAG_ANTIALIAS},
		{ 0x0, "Verdana", 12, 800, FONTFLAG_DROPSHADOW},
	 });
	F::Menu.ConfigLoaded = true;
}


LONG WINAPI UnhandledExFilter(PEXCEPTION_POINTERS ExPtr);

DWORD WINAPI MainThread(LPVOID lpParam)
{
	//AddVectoredExceptionHandler(0, UnhandledExFilter);

	//"mss32.dll" being one of the last modules to be loaded
	//So wait for that before proceeding, after it's up everything else should be too
	//Allows us to correctly use autoinject and just start the game.
	while (!GetModuleHandleW(L"mss32.dll") ||
		   !GetModuleHandleW(L"ntdll.dll") ||
		   !GetModuleHandleW(L"stdshader_dx9.dll") ||
		   !GetModuleHandleW(L"materialsystem.dll"))
	{
		Sleep(5000);
	}

	Initialize();
	LoadDefaultConfig();

	g_Events.Setup({ "vote_cast", "player_changeclass", "player_connect", "player_hurt", "achievement_earned", "player_death", "vote_started", "teamplay_round_start", "player_spawn", "item_pickup" }); // all events @ https://github.com/tf2cheater2013/gameevents.txt

	Loaded();

	while (!GetAsyncKeyState(VK_F11) || F::Menu.IsOpen)
	{
		Sleep(20);
	}

	Uninitialize();

	FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), EXIT_SUCCESS);
	return EXIT_SUCCESS;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		Utils::RemovePEH(hinstDLL);
		if (const auto hMainThread = CreateThread(nullptr, 0, MainThread, hinstDLL, 0, nullptr))
		{
			CloseHandle(hMainThread);
		}
	}

	return TRUE;
}

////https://www.unknowncheats.me/forum/c-and-c-/63409-write-mindump-crash.html
//
//#include <dbghelp.h>
//#include <shlobj.h>
//#include <tchar.h>
//
//LONG WINAPI UnhandledExFilter(PEXCEPTION_POINTERS ExPtr)
//{
//	BOOL(WINAPI * pMiniDumpWriteDump)(IN HANDLE hProcess, IN DWORD ProcessId, IN HANDLE hFile, IN MINIDUMP_TYPE DumpType, IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL) = NULL;
//
//	HMODULE hLib = LoadLibrary(_T("dbghelp"));
//	if (hLib)
//		*(void**)&pMiniDumpWriteDump = (void*)GetProcAddress(hLib, "MiniDumpWriteDump");
//
//	TCHAR buf[MAX_PATH], buf2[MAX_PATH];
//
//	if (pMiniDumpWriteDump)
//	{
//		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buf);
//		int rnd;
//		__asm push edx
//		__asm rdtsc
//		__asm pop edx
//		__asm mov rnd, eax
//		rnd &= 0xFFFF;
//		wsprintfW(buf2, _T("%s\\Fedoraware_CrashDump_%x%x%x.dmp"), buf, rnd, rnd, rnd);
//		HANDLE hFile = CreateFile(buf2, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//
//		if (hFile != INVALID_HANDLE_VALUE)
//		{
//			MINIDUMP_EXCEPTION_INFORMATION md;
//			md.ThreadId = GetCurrentThreadId();
//			md.ExceptionPointers = ExPtr;
//			md.ClientPointers = FALSE;
//			BOOL win = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &md, 0, 0);
//
//			if (!win)
//				wsprintfW(buf, _T("MiniDumpWriteDump failed. Error: %u \n(%s)"), GetLastError(), buf2);
//			else
//				wsprintfW(buf, _T("Minidump created:\n%s"), buf2);
//			CloseHandle(hFile);
//
//		}
//		else
//		{
//			wsprintfW(buf, _T("Could not create minidump:\n%s"), buf2);
//		}
//	}
//	else
//	{
//		wsprintf(buf, _T("Could not load dbghelp"));
//	}
//	MessageBoxW(NULL, buf, _T("Dump file on desktop, make an issue"), MB_OK | MB_ICONERROR);
//	abort();
//}
