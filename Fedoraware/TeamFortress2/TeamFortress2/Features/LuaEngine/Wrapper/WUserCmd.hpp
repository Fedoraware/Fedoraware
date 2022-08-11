#pragma once

class WUserCmd {
	CUserCmd* UserCmd;
	bool* SendPacket;

public:
	explicit WUserCmd(CUserCmd* userCmd, bool* sendPacket) : UserCmd(userCmd), SendPacket(sendPacket) { }

	bool IsValid()
	{
		return UserCmd != nullptr && SendPacket != nullptr;
	}

	int GetButtons()
	{
		if (!IsValid()) { return 0; }
		return UserCmd->buttons;
	}

	Vec3 GetViewAngles()
	{
		if (!IsValid()) { return { }; }
		return UserCmd->viewangles;
	}

	float GetForwardMove()
	{
		if (!IsValid()) { return 0.f; }
		return UserCmd->forwardmove;
	}

	float GetSideMove()
	{
		if (!IsValid()) { return 0.f; }
		return UserCmd->sidemove;
	}

	float GetUpMove()
	{
		if (!IsValid()) { return 0.f; }
		return UserCmd->upmove;
	}

	void SetButtons(int buttons)
	{
		if (!IsValid()) { return; }
		UserCmd->buttons = buttons;
	}

	void SetViewAngles(const Vec3& va)
	{
		if (!IsValid()) { return; }
		UserCmd->viewangles = va;
	}

	void SetForwardMove(float f)
	{
		if (!IsValid()) { return; }
		UserCmd->forwardmove = f;
	}

	void SetSideMove(float f)
	{
		if (!IsValid()) { return; }
		UserCmd->sidemove = f;
	}

	void SetUpMove(float f)
	{
		if (!IsValid()) { return; }
		UserCmd->upmove = f;
	}

	void SetSendPacket(bool state)
	{
		if (!IsValid()) { return; }
		*SendPacket = state;
	}
};
