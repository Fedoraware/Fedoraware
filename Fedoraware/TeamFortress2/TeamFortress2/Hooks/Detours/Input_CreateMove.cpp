#include "../Hooks.h"

inline CUserCmd* GetCmds()
{
	return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(I::Input) + 0xFC);
}

MAKE_HOOK(Input_CreateMove, Utils::GetVFuncPtr(I::Input, 3), void, __fastcall, IInput* input, void* edx, int sequence_number, float input_sample_time, bool active)
{
	// Call original first, this also includes the normal CreateMove
	Hook.Original<FN>()(input, edx, sequence_number, input_sample_time, active);

	CUserCmd* pCmd = nullptr;
	const auto cmds = GetCmds();

	if (sequence_number > 0 && cmds)
	{
		pCmd = input->GetUserCmd(sequence_number);
	}

	if (!pCmd) { return; }
	G::LateUserCmd = pCmd;

	// Write the UserCmd
	const auto verifiedCmds = *reinterpret_cast<CVerifiedUserCmd**>(reinterpret_cast<uintptr_t>(input) + 0x100);
	verifiedCmds[sequence_number % 90].m_cmd = *pCmd;
	verifiedCmds[sequence_number % 90].m_crc = pCmd->GetChecksum();
	cmds[sequence_number % 90] = *pCmd;
}