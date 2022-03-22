#pragma once

#include "../../Includes/Includes.h"

class CNetChannel;

class ISpatialLeafEnumerator
{
public:
	virtual bool EnumerateLeaf(int leaf, int context) = 0;
};

class ISpatialQuery
{
public:
	virtual int LeafCount() const = 0;
	virtual bool EnumerateLeavesAtPoint(Vec3 const &pt, ISpatialLeafEnumerator *pEnum, int context) = 0;
	virtual bool EnumerateLeavesInBox(Vec3 const &mins, Vec3 const &maxs, ISpatialLeafEnumerator *pEnum, int context) = 0;
	virtual bool EnumerateLeavesInSphere(Vec3 const &center, float radius, ISpatialLeafEnumerator *pEnum, int context) = 0;
	virtual bool EnumerateLeavesAlongRay(Ray_t const &ray, ISpatialLeafEnumerator *pEnum, int context) = 0;
};

class CEngineClient
{
public:
	void GetScreenSize(int &width_out, int &height_out)
	{
		typedef void(__thiscall *FN)(void*, int &, int &);
		return GetVFunc<FN>(this, 5)(this, width_out, height_out);
	}

	void ServerCmd(const char *szCmdString, bool bReliable)
	{
		typedef void(__thiscall *FN)(void*, const char *, bool);
		return GetVFunc<FN>(this, 6)(this, szCmdString, bReliable);
	}

	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t *pPlayerInfo)
	{
		typedef bool(__thiscall *FN)(void*, int, PlayerInfo_t *);
		return GetVFunc<FN>(this, 8)(this, nEntityIndex, pPlayerInfo);
	}

	int GetPlayerForUserID(int userID)
	{
		typedef bool(__thiscall* FN)(void*, int);
		return GetVFunc<FN>(this, 9)(this, userID);
	}

	bool Con_IsVisible()
	{
		typedef bool(__thiscall *FN)(void*);
		return GetVFunc<FN>(this, 11)(this);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 12)(this);
	}

	float Time()
	{
		typedef float(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 14)(this);
	}

	void GetViewAngles(Vec3 &va)
	{
		typedef void(__thiscall *FN)(PVOID, Vec3 &va);
		return GetVFunc<FN>(this, 19)(this, va);
	}

	Vec3 GetViewAngles()
	{
		Vec3 out;
		GetViewAngles(out);
		return out;
	}

	void SetViewAngles(Vec3 &va)
	{
		typedef void(__thiscall *FN)(PVOID, Vec3 &va);
		return GetVFunc<FN>(this, 20)(this, va);
	}

	int GetMaxClients()
	{
		typedef int(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 21)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 26)(this);
	}

	bool IsConnected()
	{
		typedef bool(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 27)(this);
	}

	bool IsDrawingLoadingImage()
	{
		typedef bool(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 28)(this);
	}

	const matrix3x4 &WorldToScreenMatrix()
	{
		typedef const matrix3x4 &(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 36)(this);
	}

	void FireEvents()
	{
		typedef void(__thiscall* FN)(PVOID);
		GetVFunc<FN>(this, 56)(this);
	} // this triggers @https://github.com/lua9520/source-engine-2018-hl2_src/blob/3bf9df6b2785fa6d951086978a3e66f49427166a/engine/cl_parse_event.cpp#L75
	// TODO: find a way to bypass the "Delayed Event!" check, so we send all events instantly...

	bool IsTakingScreenshot()
	{
		typedef bool(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 85)(this);
	}

	bool IsHLTV()
	{
		typedef bool(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 86)(this);
	}

	CNetChannel* GetNetChannelInfo()
	{
		typedef CNetChannel* (__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 72)(this);
	}

	void ClientCmd_Unrestricted(const char *szCommandString)
	{
		typedef void(__thiscall *FN)(PVOID, const char *);
		return GetVFunc<FN>(this, 106)(this, szCommandString);
	}

	void ServerCmdKeyValues(void* pKV)
	{
		typedef void(__thiscall* FN)(void*, void*);
		GetVFunc<FN>(this, 127)(this, pKV);
		//using fn = void(__stdcall*)(void*); // KeyValues* not void* but I don't know what consequences including KeyValues.h will have here
		//static fn ServerCmdKeyValuesFn = reinterpret_cast<fn>(g_Pattern.Find(L"engine.dll", L"55 8B EC B9 ? ? ? ? 5D E9 ? ? ? ? CC CC 55 8B EC 8A 45 08"));
		//ServerCmdKeyValuesFn(pKV);
	}
		
	ISpatialQuery *GetBSPTreeQuery()
	{
		typedef ISpatialQuery *(__thiscall *FN)(PVOID);
		return GetVFunc<FN>(this, 42)(this);
	}

	char const* GetLevelName()
	{
		typedef char const* (__thiscall* FN)(PVOID);
		return GetVFunc<FN>(this, 51)(this);
	}
};

#define VENGINE_CLIENT_INTERFACE_VERSION_13	"VEngineClient013"