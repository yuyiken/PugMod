#include "precompiled.h"

CPugGameDesc gPugGameDesc;

void CPugGameDesc::Update()
{
    if (g_pGameRules)
    {
        static char GameDesc[32];

        if (gPugCvar.m_GameDesc->value > 0.0f)
        {
            auto State = gPugMod.GetState();

            if (State >= STATE_DEATHMATCH && State <= STATE_KNIFE_ROUND)
            {
                Q_snprintf(GameDesc, sizeof(GameDesc), "%s", _T(g_Pug_String[State]));
            }
            else if (State >= STATE_FIRST_HALF && State <= STATE_END)
            {
                Q_snprintf(GameDesc, sizeof(GameDesc), "%s %d : %d %s", g_Pug_TeamNameShort[TERRORIST], gPugMod.GetScore(TERRORIST), gPugMod.GetScore(CT), g_Pug_TeamNameShort[CT]);
            }
            else
            {
                Q_snprintf(GameDesc, sizeof(GameDesc), "Counter-Strike");
            }
        }
        else
        {
            Q_snprintf(GameDesc, sizeof(GameDesc), "Counter-Strike");
        }

        g_ReGameFuncs->ChangeString(g_pGameRules->m_GameDesc, GameDesc);
    }
}