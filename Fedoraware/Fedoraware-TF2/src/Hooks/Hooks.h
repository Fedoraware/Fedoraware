#pragma once
#include <MinHook/MinHook.h>
#include <stdexcept>

#include "../SDK/SDK.h"

//Credits go entirely to spook953

class CHook
{
	std::string m_Name;
	void* m_OriginalFunction = nullptr;
	void* m_InitFunction = nullptr;

public:
	CHook(const std::string& name, void* pInitFunction);

	void CreateHook(void* pTarget, void* pDetour)
	{
		if (MH_CreateHook(pTarget, pDetour, &m_OriginalFunction) != MH_OK)
		{
			throw std::runtime_error("Failed to create hook: " + m_Name);
		}
	}

	void DisableHook()
	{
		if (MH_DisableHook(m_OriginalFunction) != MH_OK)
		{
			throw std::runtime_error("Failed to disable hook: " + m_Name);
		}
	}

	void Init()
	{
		reinterpret_cast<void(__cdecl*)()>(m_InitFunction)();
	}

	template <typename FN>
	FN Original()
	{
		return reinterpret_cast<FN>(m_OriginalFunction);
	}
};

#define MAKE_HOOK(name, address, type, callconvo, ...) namespace Hooks \
{\
	namespace name\
	{\
		void Initialize();\
		inline CHook Hook(#name, Initialize); \
		using FN = type(callconvo*)(__VA_ARGS__); \
		type callconvo Detour(__VA_ARGS__); \
	}\
} \
void Hooks::name::Initialize() { Hook.CreateHook(reinterpret_cast<void*>(address), Detour); } \
type callconvo Hooks::name::Detour(__VA_ARGS__)