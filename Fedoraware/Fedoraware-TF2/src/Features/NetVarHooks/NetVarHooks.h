#pragma once
#include "../Feature.h"

#include "../../SDK/Includes/proxyfnhook.h"
#include "../Vars.h"

class NetVarHooks
{
private:
	bool HookNetvar(const std::vector<std::string>& path, ProxyFnHook& hook, RecvVarProxyFn function);

public:
	void Init();
};

ADD_FEATURE(NetVarHooks, NetHooks)
