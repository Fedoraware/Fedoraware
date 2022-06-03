#include "../Hooks.h"
#include "../../Features/NoSpread/NoSpread.h"

#define VERIFIED_CMD_SIZE 90

inline CVerifiedUserCmd* GetVerifiedCmds(IInput* input)
{
	return *reinterpret_cast<CVerifiedUserCmd**>(reinterpret_cast<uintptr_t>(input) + 0x100);
}

inline CUserCmd* GetCmds(IInput* input)
{
	return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(input) + 0xFC);
}

void WriteCmd(IInput* input, const CUserCmd* cmd, int sequence_nr)
{
	// Write the usercmd
	GetVerifiedCmds(input)[sequence_nr % VERIFIED_CMD_SIZE].m_cmd = *cmd;
	GetVerifiedCmds(input)[sequence_nr % VERIFIED_CMD_SIZE].m_crc = cmd->GetChecksum();
	GetCmds(input)[sequence_nr % VERIFIED_CMD_SIZE] = *cmd;
}

MAKE_HOOK(Input_CreateMove, Utils::GetVFuncPtr(I::Input, 3), void, __fastcall, IInput* input, void* edx, int sequence_number, float input_sample_time, bool active)
{
	// Call original first, this also includes the normal CreateMove
	Hook.Original<FN>()(input, edx, sequence_number, input_sample_time, active);

	CUserCmd* pCmd = nullptr;
	const auto cmds = *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(input) + 0xFC);

	if (sequence_number > 0 && cmds)
	{
		pCmd = input->GetUserCmd(sequence_number);
	}

	if (!pCmd) { return; }
	G::LateUserCmd = pCmd;

	// Run late features
	{
		F::NoSpread.RunLate(pCmd);
	}

	// Write the UserCmd
	WriteCmd(input, pCmd, sequence_number);
}