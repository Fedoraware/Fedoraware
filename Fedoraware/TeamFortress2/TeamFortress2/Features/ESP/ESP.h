#pragma once
#include "../../SDK/SDK.h"

class CESP
{
private:
	static bool ShouldRun();
	std::vector<std::wstring> GetPlayerConds(CBaseEntity* pEntity) const;
	static const wchar_t* GetPlayerClass(int nClassNum);

	void DrawPlayers(CBaseEntity* pLocal);
	void DrawBuildings(CBaseEntity* pLocal) const;
	void DrawWorld() const;
	static void Draw3DBox(Vec3* vPoints, Color_t clr);
	static void CreateDLight(int nIndex, Color_t DrawColor, const Vec3& vOrigin, float flRadius);
	static void DrawBones(CBaseEntity* pPlayer, const std::vector<int>& vecBones, Color_t clr);

public:
	void Run();
	static bool GetDrawBounds(CBaseEntity* pEntity, Vec3* vTrans, int& x, int& y, int& w, int& h);
};

inline CESP g_ESP;
