#include <Windows.h>
#include "Core/Core.h"

void RemovePEH(HINSTANCE hinstDLL)
{
	const auto pImageDOS = reinterpret_cast<PIMAGE_DOS_HEADER>(hinstDLL);

	// Was the header already manipulated?
	if (pImageDOS->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}
	
	const auto pImageNT = reinterpret_cast<PIMAGE_NT_HEADERS>(pImageDOS + pImageDOS->e_lfanew);
	const auto sizeOfPe = pImageNT->FileHeader.SizeOfOptionalHeader;

	if (pImageNT->Signature == IMAGE_NT_SIGNATURE && sizeOfPe)
	{
		const auto headerSize = static_cast<size_t>(sizeOfPe);
		DWORD dwProtect = 0x0;

		// Zero out the header
		VirtualProtect(hinstDLL, headerSize, PAGE_EXECUTE_READWRITE, &dwProtect);
		RtlZeroMemory(hinstDLL, headerSize);
		VirtualProtect(hinstDLL, headerSize, dwProtect, &dwProtect);
	}
}

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

	while (!g_Core.ShouldUnload())
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

		RemovePEH(hinstDLL);
		DisableThreadLibraryCalls(hinstDLL);
		if (const auto hMainThread = CreateThread(nullptr, 0, MainThread, hinstDLL, 0, nullptr))
		{
			CloseHandle(hMainThread);
		}
	}

	return TRUE;
}
