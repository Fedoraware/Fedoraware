#pragma once
#include "../../SDK/SDK.h"

class CESP
{
private:
	bool ShouldRun();
	bool GetDrawBounds(CBaseEntity* pEntity, Vec3* vTrans, int& x, int& y, int& w, int& h);
	std::wstring GetPlayerConds(CBaseEntity* pEntity);
	const wchar_t* GetPlayerClass(int nClassNum);

	void DrawPlayers(CBaseEntity* pLocal);
	void DrawBuildings(CBaseEntity* pLocal);
	void DrawWorld();
	void Draw3DBox(Vec3* vPoints, Color_t clr);
	void CreateDLight(int nIndex, Color_t DrawColor, const Vec3& vOrigin, float flRadius);
	void DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr);

public:
	void Run();
};

inline CESP g_ESP;