#pragma once

#include "../../../SDK/SDK.h"

class CMovementSimulation
{
public:
    CBaseEntity* m_pPlayer = nullptr;
    CMoveData m_MoveData = {};

private:
    //void SetCurrentCommand(CBaseEntity* pPlayer, CUserCmd* pCmd);
    void SetupMoveData(CBaseEntity* pPlayer, CMoveData* pMoveData);

private:
    bool m_bOldInPrediction = false;
    bool m_bOldFirstTimePredicted = false;
    float m_flOldFrametime = 0.0f;

public:
    bool Initialize(CBaseEntity* pPlayer);
    bool InitializeForProjaim(CBaseEntity* pPlayer);
    void Restore();
    void RunTick(CMoveData& moveDataOut, Vec3& worldSpaceCenterOut);
    inline const Vec3& GetOrigin() { return m_MoveData.m_vecAbsOrigin; }
};

inline CMovementSimulation g_MoveSim;