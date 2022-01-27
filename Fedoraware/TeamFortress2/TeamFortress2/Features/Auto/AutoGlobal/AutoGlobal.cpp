#include "AutoGlobal.h"

#include "../../Vars.h"

bool CAutoGlobal::IsKeyDown()
{
	switch (Vars::Triggerbot::Global::TriggerKey.m_Var)
	{
	case 0x0: return true;
	default: return (GetAsyncKeyState(Vars::Triggerbot::Global::TriggerKey.m_Var) & 0x8000);
	}
}