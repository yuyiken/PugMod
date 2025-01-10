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

        auto Players = gPugUtil.GetPlayers(false, false);

        for (auto Player : Players)
        {
            gPugUtil.ScreenShake(Player->edict(), 2.0f, 2.0f, 2.0f);

            gPugUtil.ClientCommand(Player->edict(), g_LO3_Sound[this->m_Restart]);

            gPugUtil.ScreenFade(Player->edict(), 2.0f, 2.0f, 0x0002, 0, 0, 200, 100);

            gPugUtil.SendHud(Player->edict(), g_LO3_HudParam, g_LO3_HudText[this->m_Restart]);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, g_LO3_Message[this->m_Restart], gPugCvar.m_Tag->string);
        }

        this->m_Restart += 1;
    }
}