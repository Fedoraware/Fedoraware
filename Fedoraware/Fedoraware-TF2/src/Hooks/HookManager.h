#pragma once
#include <unordered_map>
#include <string>

class CHook;

class CHookManager
{
public:
	void Init();
	void Release();

	std::unordered_map<std::string, CHook*>& GetMapHooks()
	{
		static std::unordered_map<std::string, CHook*> mapHooks;
		return mapHooks;
	}
};

inline CHookManager g_HookManager;