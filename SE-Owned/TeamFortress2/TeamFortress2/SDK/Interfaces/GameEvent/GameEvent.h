#pragma once

class CGameEvent
{
public:
	virtual ~CGameEvent() { };
	virtual const char* GetName() const = 0;

	virtual bool IsReliable() const = 0;
	virtual bool IsLocal() const = 0;
	virtual bool IsEmpty(const char* szKey = 0) = 0;

	virtual bool GetBool(const char* szKey = 0, bool bDefault = false) = 0;
	virtual int	GetInt(const char* szKey = 0, int nDefault = 0) = 0;
	virtual float GetFloat(const char* szKey = 0, float flDefault = 0.0f) = 0;
	virtual const char* GetString(const char* szKey = 0, const char* szDefault = "") = 0;

	virtual void SetBool(const char* szKey, bool bNew) = 0;
	virtual void SetInt(const char* szKey, int nNew) = 0;
	virtual void SetFloat(const char* szKey, float flNew) = 0;
	virtual void SetString(const char* szKey, const char* szNew) = 0;
};

class CGameEventListener
{
public:
	virtual ~CGameEventListener(void) {};

	virtual void FireGameEvent(CGameEvent* pEvent) = 0;
};

class CGameEventManager
{
public:
	bool AddListener(CGameEventListener* pListener, const char* szName, bool ServerSide) 
	{
		typedef bool(__thiscall* FN)(void*, CGameEventListener*, const char*, bool);
		return GetVFunc<FN>(this, 3)(this, pListener, szName, ServerSide);
	}

	void RemoveListener(CGameEventListener* pListener)
	{
		typedef bool(__thiscall* FN)(void*, CGameEventListener*);
		GetVFunc<FN>(this, 5)(this, pListener);
	}
};

#define GAMEEVENTSMANAGER_ENGINE_INTERFACE "GAMEEVENTSMANAGER002"