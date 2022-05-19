#pragma once

#include "../Utils/MinHook/MinHook.h"
#include "../SDK/SDK.h"
#include <stdexcept>

class CHook
{
private:
	void* m_pOriginalFunction = nullptr;

public:
	CHook(void* pInitFunction);

	void CreateHook(void* pTarget, void* pDetour)
	{
		if (MH_CreateHook(pTarget, pDetour, &m_pOriginalFunction) != MH_STATUS::MH_OK)
		{
			throw std::runtime_error("Failed to create hook");
		}
	}

	void DisableHook()
	{
		if (MH_DisableHook(m_pOriginalFunction) != MH_STATUS::MH_OK)
		{
			throw std::runtime_error("Failed to disable hook");
		}
	}

	template <typename FN>
	inline FN Original()
	{
		return reinterpret_cast<FN>(m_pOriginalFunction);
	}
};

#define MAKE_HOOK(name, address, type, callconvo, ...) namespace Hooks \
{\
	namespace name\
	{\
		void Initialize();\
		inline CHook Hook(Initialize); \
		using FN = type(callconvo*)(__VA_ARGS__); \
		type callconvo Detour(__VA_ARGS__); \
	}\
} \
void Hooks::name::Initialize() { Hook.CreateHook(reinterpret_cast<void*>(address), Detour); } \
type callconvo Hooks::name::Detour(__VA_ARGS__)