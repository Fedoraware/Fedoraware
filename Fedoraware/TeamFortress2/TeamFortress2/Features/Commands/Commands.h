#pragma once
#include <functional>

#include "../../SDK/SDK.h"

using CommandCallback = std::function<void(std::deque<std::string>)>;

class CCommands {
	std::unordered_map<std::string, CommandCallback> CommandMap;

public:
	void Init();
	bool Run(const std::string& cmd, const std::deque<std::string>& args);
	void Register(const std::string& name, CommandCallback callback);
};

ADD_FEATURE(CCommands, Commands)