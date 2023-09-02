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
	virtual int GetEventDebugID() { return m_nDebug; }

private:
	int m_nDebug;
};

class CSVCMsg_GameEvent;
class CGameEventManager
{
public:
	virtual				~CGameEventManager() { }
	virtual int			LoadEventsFromFile(const char* szFileName) = 0;
	virtual void		Reset() = 0;
	virtual bool		AddListener(CGameEventListener* pListener, const char* szName, bool bServerSide) = 0;
	virtual bool		FindListener(CGameEventListener* pListener, const char* szName) = 0;
	virtual void		RemoveListener(CGameEventListener* pListener) = 0;
	virtual void		AddListenerGlobal(CGameEventListener* pListener, bool bServerSide) = 0;

	// virtual CGameEvent* CreateNewEvent(const char* szName, bool bForce = false, int* unknown = nullptr) = 0;
	CGameEvent* CreateNewEvent(const char* szName, bool bForce = false) {
		return GetVFunc<CGameEvent* (__thiscall*)(void*, const char*, bool)>(this, 6)(this, szName, bForce);
	}

	virtual bool		FireEvent(CGameEvent* pEvent, bool bDontBroadcast = false) = 0;
	virtual bool		FireEventClientSide(CGameEvent* pEvent) = 0;
	virtual CGameEvent* DuplicateEvent(CGameEvent* pEvent) = 0;
	virtual void		FreeEvent(CGameEvent* pEvent) = 0;
	virtual bool		SerializeEvent(CGameEvent* pEvent, CSVCMsg_GameEvent* pEventMsg) = 0;
	virtual CGameEvent* UnserializeEvent(const CSVCMsg_GameEvent& eventMsg) = 0;
	virtual void* GetEventDataTypes(CGameEvent* pEvent) = 0;
};

#define GAMEEVENTSMANAGER_ENGINE_INTERFACE "GAMEEVENTSMANAGER002"