#pragma once
#include <format>
#include "../Utils/Pattern/Pattern.h"

constexpr auto CLIENT_DLL = "client.dll";
constexpr auto ENGINE_DLL = "engine.dll";
constexpr auto VSTDLIB_DLL = "vstdlib.dll";

class CSignature
{
	DWORD m_Address = 0;
	LPCSTR m_Module = nullptr;
	LPCSTR m_Pattern = nullptr;
	int m_Offset = 0;

	// Find and cache the address
	void Find()
	{
		m_Address = g_Pattern.Find(m_Module, m_Pattern);
		if (m_Address == 0)
		{
			const auto msg = std::format("Module: {}\nPattern: {}\nAddress: {:#x} (+{:d})", m_Module, m_Pattern, m_Address, m_Offset);
			MessageBoxA(nullptr, msg.c_str(), "Pattern not found", MB_OK | MB_ICONERROR);
		}
	}

public:
	CSignature(LPCSTR szModule, LPCSTR szPattern, int offset = 0)
		: m_Module(szModule), m_Pattern(szPattern), m_Offset(offset) { }

	// Return the address
	DWORD operator()()
	{
		if (m_Address == 0) { Find(); }
		return m_Address + m_Offset;
	}

	template <typename T> T* As() { return reinterpret_cast<T*>(this->operator()()); }
};

#define MAKE_SIGNATURE(name, module, pattern, offset) inline CSignature name{ module, pattern, offset }

// Signatures
namespace S
{
	// Interfaces
	MAKE_SIGNATURE(GlobalVars_Interface, ENGINE_DLL, "A1 ? ? ? ? 8B 11 68", 0x8);
	MAKE_SIGNATURE(ClientState_Interface, ENGINE_DLL, "68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3", 0x1);
	MAKE_SIGNATURE(DemoPlayer_Interface, ENGINE_DLL, "8B 0D ? ? ? ? 85 C9 74 3B 8B 01 8B 40 18 FF D0 84 C0 74 30", 0x2);
	MAKE_SIGNATURE(TFGCClientSystem_Interface, CLIENT_DLL, "B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D F8", 0x1);
	MAKE_SIGNATURE(TFInventoryManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ?", 0x1);
	MAKE_SIGNATURE(Input_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC", 0x2);
	MAKE_SIGNATURE(UniformRandomStream_Interface, VSTDLIB_DLL, "B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3", 0x1);
	MAKE_SIGNATURE(ViewRenderBeams_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?", 0x2);
	MAKE_SIGNATURE(TFGameRules_Interface, CLIENT_DLL, "8B 0D ? ? ? ? 56 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 84 ? ? ? ? 80 BB ? ? ? ? ?", 0x1);
	MAKE_SIGNATURE(ThirdPersonManager_Interface, CLIENT_DLL, "B9 ? ? ? ? E8 ? ? ? ? 84 C0 74 42 8B 86", 0x1);
	MAKE_SIGNATURE(ClientModeTFNormal_Interface, CLIENT_DLL, "B9 ? ? ? ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 83 C4 04 8B 35 ? ? ? ?", 0x1);
	MAKE_SIGNATURE(HostState_Interface, ENGINE_DLL, "8B 15 ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85 ? ? ? ? ? C6 85", 0x1);

	MAKE_SIGNATURE(DirectXDevice, "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C", 0x1);

	// Values
	MAKE_SIGNATURE(RandomSeed, CLIENT_DLL, "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34", 0x2);
	MAKE_SIGNATURE(AllowSecureServers, ENGINE_DLL, "C6 05 ? ? ? ? ? 8A C3", 0x2);
}
