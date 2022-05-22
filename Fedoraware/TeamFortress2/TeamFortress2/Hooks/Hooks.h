#pragma once

#include "../Utils/MinHook/MinHook.h"
#include "../SDK/SDK.h"
#include <stdexcept>

//Credits go entirely to spook953

class CHook
{
	std::string Name;
	void* OriginalFunction = nullptr;
	void* InitFunction = nullptr;

public:
	CHook(const std::string& name, void* pInitFunction);

	void CreateHook(void* pTarget, void* pDetour)
	{
		if (MH_CreateHook(pTarget, pDetour, &OriginalFunction) != MH_OK)
		{
			throw std::runtime_error("Failed to create hook: " + Name);
		}
	}

	void DisableHook()
	{
		if (MH_DisableHook(OriginalFunction) != MH_OK)
		{
			throw std::runtime_error("Failed to disable hook: " + Name);
		}
	}

	void Init()
	{
		reinterpret_cast<void(__cdecl*)()>(InitFunction)();
	}

	template <typename FN>
	FN Original()
	{
		return reinterpret_cast<FN>(OriginalFunction);
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