#pragma once

class WGameEvent {
	CGameEvent* GameEvent = nullptr;

public:
	explicit WGameEvent(CGameEvent* gameEvent) : GameEvent(gameEvent) { }

	bool IsValid()
	{
		return GameEvent != nullptr;
	}

	const char* GetName()
	{
		if (!IsValid()) { return ""; }
		return GameEvent->GetName();
	}

	bool GetBool(const char* szKey)
	{
		if (!IsValid()) { return false; }
		return GameEvent->GetBool(szKey);
	}

	int GetInt(const char* szKey)
	{
		if (!IsValid()) { return 0; }
		return GameEvent->GetInt(szKey);
	}

	float GetFloat(const char* szKey)
	{
		if (!IsValid()) { return 0.f; }
		return GameEvent->GetFloat(szKey);
	}

	const char* GetString(const char* szKey)
	{
		if (!IsValid()) { return ""; }
		return GameEvent->GetString(szKey);
	}

	void SetBool(const char* szKey, bool bNew)
	{
		if (!IsValid()) { return; }
		GameEvent->SetBool(szKey, bNew);
	}

	void SetInt(const char* szKey, int bNew)
	{
		if (!IsValid()) { return; }
		GameEvent->SetInt(szKey, bNew);
	}

	void SetFloat(const char* szKey, float bNew)
	{
		if (!IsValid()) { return; }
		GameEvent->SetFloat(szKey, bNew);
	}

	void SetString(const char* szKey, const char* bNew)
	{
		if (!IsValid()) { return; }
		GameEvent->SetString(szKey, bNew);
	}
};