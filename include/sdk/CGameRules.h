/*
 * CGameRules.h
 *
 *  Created on: Nov 23, 2017
 *      Author: nullifiedcat
 */

#pragma once
class CGameRules
{
public:
    int pad0[12];               // 0    | 48 bytes  | 48
    int roundmode;              // 48   | 4 bytes   | 52
    int m_bInWaitingForPlayers; // 52   | 4 bytes   | 56
    int winning_team;           // 56   | 4 bytes   | 60
    char pad2[974];             // 60   | 974 bytes | 1034
    bool isPVEMode;             // 1034 | 1 byte    | 1035
    char pad3[15];              // 1035 | 15 bytes  | 1050
    int isUsingSpells;          // 1050 | 4 bytes   | 1054
    char pad4[790];             // 1054 | 790 bytes | 1844
    int halloweenScenario;      // 1844 | 4 bytes   | 1848

    bool isUsingSpells_fn()
    {
        auto tf_spells_enabled = g_ICvar->FindVar("tf_spells_enabled");
        if (tf_spells_enabled->GetBool())
            return true;

        // Hightower
        if (halloweenScenario == 4)
            return true;

        return isUsingSpells;
    }
} __attribute__((packed));
