#include "precompiled.h"

CPugLO3 gPugLO3;

void CPugLO3::Init()
{
    this->m_Run = true;
    
    this->m_Restart = 0;

    this->RestartRound();
}

void CPugLO3::RestartRound()
{
    if (this->m_Run)
    {
        if (this->m_Restart <= 3)
        {
            if (g_pGameRules)
            {
                CSGameRules()->m_bGameStarted = true;
                CSGameRules()->m_bCompleteReset = true;
                CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + 3.0f);
            }
        }
        else
        {
            this->m_Run = false;
        }

        auto State = gPugMod.GetState();

        edict_t *pEntity = nullptr;

        for (int i = 1; i <= gpGlobals->maxClients; ++i)
        {
            pEntity = INDEXENT(i);

            if (gPugUtil.IsNetClient(pEntity))
            {
                gPugUtil.ScreenShake(pEntity, 2.0f, 2.0f, 2.0f);

                gPugUtil.ClientCommand(pEntity, g_LO3_Sound[this->m_Restart]);

                gPugUtil.ScreenFade(pEntity, 2.0f, 2.0f, 0x0002, 0, 0, 200, 100);

                gPugUtil.SendHud(pEntity, g_LO3_HudParam, g_LO3_HudText[this->m_Restart]);

                gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, g_LO3_Message[this->m_Restart], gPugCvar.m_Tag->string, g_Pug_String[State]);
            }
        }

        this->m_Restart += 1;
    }
}