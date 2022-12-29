#include "../Hooks.h"
#include "../../Features/TickHandler/TickHandler.h"

//	"NetMsg"
//	@net_chan.cpp L2524
MAKE_HOOK(CNetChan_SendNetMsg, g_Pattern.Find(L"engine.dll", L"55 8B EC 57 8B F9 8D 8F ? ? ? ? E8 ? ? ? ? 85 C0 75 07 B0 01 5F 5D C2 0C 00"), bool, __fastcall,
		  CNetChannel* netChannel, void* edi, INetMessage& msg, bool bForceReliable, bool bVoice)
{
	switch (msg.GetType())
	{
		case clc_VoiceData:
		{
			// stop lag with voice chat
			bVoice = true;
			break;
		}

		case clc_FileCRCCheck:
		{
			// whitelist
			if (Vars::Misc::BypassPure.Value)
			{
				return false;
			}
			break;
		}

		case clc_RespondCvarValue:
		{
			//	causes b1g crash
			if (Vars::Visuals::RemoveConvarQueries.Value)
			{
				if (const auto respondMsg = reinterpret_cast<DWORD*>(&msg))
				{
					if (const auto cvarName = reinterpret_cast<const char*>(respondMsg[6]))
					{
						if (const auto convarC = g_ConVars.FindVar(cvarName))
						{
							if (const char* defaultValue = convarC->GetDefault())
							{
								respondMsg[7] = reinterpret_cast<DWORD>(defaultValue);
								I::Cvar->ConsoleColorPrintf({ 255, 0, 0, 255 }, "%s\n", msg.ToString()); //	mt everest
								break;
							}
						}
						return true; //	if we failed to manipulate the data, don't send it.
					}
				}
			}
			break;
		}

		case clc_Move:
		{
			static int iOldShift = G::ShiftedTicks;
			const int iAllowedNewCommands = fmax(fmin(24 - G::ShiftedTicks, 22), 0);
			const auto& moveMsg = reinterpret_cast<CLC_Move&>(msg);
			const int iCmdCount = moveMsg.m_nNewCommands + (moveMsg.m_nNewCommands > 14 ? moveMsg.m_nBackupCommands : 0);
			if (iCmdCount > iAllowedNewCommands)
			{
				Utils::ConLog("clc_Move", tfm::format("%d sent <%d | %d>, max was %d.", iCmdCount, moveMsg.m_nNewCommands, moveMsg.m_nBackupCommands, iAllowedNewCommands).c_str(), { 0, 222, 255, 255 });
				G::ShiftedTicks -= iCmdCount - iAllowedNewCommands;
				F::Ticks.iDeficit = iCmdCount - iAllowedNewCommands;
			}
			F::Ticks.iPredicted -= iCmdCount;
			iOldShift = G::ShiftedTicks;
			break;
		}
	}

	return Hook.Original<FN>()(netChannel, edi, msg, bForceReliable, bVoice);
}
