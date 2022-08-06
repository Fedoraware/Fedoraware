#pragma once

class WUserCmd {
	CUserCmd* UserCmd;
	bool* SendPacket;

public:
	explicit WUserCmd(CUserCmd* userCmd, bool* sendPacket) : UserCmd(userCmd), SendPacket(sendPacket) { }

	int GetButtons() { return UserCmd->buttons; }
	Vec3 GetViewAngles() { return UserCmd->viewangles; }
	float GetForwardMove() { return UserCmd->forwardmove; }
	float GetSideMove() { return UserCmd->sidemove; }
	float GetUpMove() { return UserCmd->upmove; }

	void SetButtons(int buttons) { UserCmd->buttons = buttons; }
	void SetViewAngles(const Vec3& va) { UserCmd->viewangles = va; }
	void SetForwardMove(float f) { UserCmd->forwardmove = f; }
	void SetSideMove(float f) { UserCmd->sidemove = f; }
	void SetUpMove(float f) { UserCmd->upmove = f; }

	void SetSendPacket(bool state) { *SendPacket = state; }
};
