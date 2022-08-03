#pragma once

class WGameEvent {
	CGameEvent* GameEvent;

public:
	explicit WGameEvent(CGameEvent* gameEvent) : GameEvent(gameEvent) { }

	const char* GetName()
	{
		return GameEvent->GetName();
	}

	bool GetBool(const char* szKey)
	{
		return GameEvent->GetBool(szKey);
	}

	int GetInt(const char* szKey)
	{
		return GameEvent->GetInt(szKey);
	}

	float GetFloat(const char* szKey)
	{
		return GameEvent->GetFloat(szKey);
	}

	const char* GetString(const char* szKey)
	{
		return GameEvent->GetString(szKey);
	}

	void SetBool(const char* szKey, bool bNew)
	{
		GameEvent->SetBool(szKey, bNew);
	}

	void SetInt(const char* szKey, int bNew)
	{
		GameEvent->SetInt(szKey, bNew);
	}

	void SetFloat(const char* szKey, float bNew)
	{
		GameEvent->SetFloat(szKey, bNew);
	}

	void SetString(const char* szKey, const char* bNew)
	{
		GameEvent->SetString(szKey, bNew);
	}
};