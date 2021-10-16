//#pragma once
//#include "../../SDK/SDK.h"
//
//class CCrits {
//private:
//public:
//	void RunInCreateMove();
//	void DrawInfo();
//};
//
//inline CCrits g_Crits;
#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include "../../SDK/SDK.h"

class Crits
{
public:
	void Tick(CUserCmd* pCommand);
	void Frame();

	std::unordered_map<int, int> new_command_number;

private:
	bool ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	void CorrectBucket(CBaseEntity* weapon, CUserCmd* pCommand);
	int FindNextCritCmdN(CBaseEntity* pWeapon, CUserCmd* pCommand);

	int CmdN = 0;
	int LastCmdN = 0;
	int LastUserCmd = 0;

	unsigned long* LastWeapon = nullptr;

	std::string CritStatus = "None";
	std::string CritStatus2 = "None";
};

extern Crits g_Crits;