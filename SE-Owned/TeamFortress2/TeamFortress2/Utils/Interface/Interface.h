#pragma once

#include <Windows.h>

#include "../WinAPI/WinAPI.h"

class CInterface
{
private:
	typedef void*(*InstantiateInterface)();

	struct Interface_t
	{
		InstantiateInterface Interface;
		PCHAR szInterfaceName;
		Interface_t *NextInterface;
	};

public:
	template<typename T>
	__inline T Get(LPCWSTR szModule, PCCH szObject)
	{
		typedef void* (*CreateFn)(const char* pName, int* pReturnCode);

		if (const auto dwModule = reinterpret_cast<DWORD>(WinAPI::GetModuleHandleW(szModule)))
		{
			if (const auto Factory = reinterpret_cast<CreateFn>(WinAPI::GetProcessAddr(dwModule, _("CreateInterface"))))
				return reinterpret_cast<T>(Factory(szObject, nullptr));
		}

		return nullptr;
	}
};

inline CInterface g_Interface;