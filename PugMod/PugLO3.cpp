#include "precompiled.h"

CPugLO3 gPugLO3;

void CPugLO3::Init()
{
    this->m_Run = true;

    this->m_NextFrame = (gpGlobals->time + 3.0f);
    
    this->m_Restart = 0;

    gPugUtil.ClientCommand(nullptr, g_LO3_Sound[0]);

    gPugUtil.SendHud(nullptr, g_LO3_HudParam, g_LO3_Message[0]);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Valendo em três restarts! ^3Fique atento !!", gPugCvar.m_Tag->string);
}

void CPugLO3::Stop()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Restart = 0;

    gPugUtil.ClientCommand(nullptr, g_LO3_Sound[4]);

    gPugUtil.SendHud(nullptr, g_LO3_HudParam, g_LO3_Message[4]);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3A partida começou! Boa Sorte & Divirta-se!!", gPugCvar.m_Tag->string);
}

void CPugLO3::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            if (this->m_Restart < 3)
            {
                this->m_Restart += 1;

                gPugUtil.ClientCommand(nullptr, g_LO3_Sound[this->m_Restart]);

                if (g_pGameRules)
                {
                    CSGameRules()->m_bGameStarted = true;
                    CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + 2.0f);
                    CSGameRules()->m_bCompleteReset = true;
                }

                gPugUtil.SendHud(nullptr, g_LO3_HudParam, g_LO3_Message[this->m_Restart]);
            }
            else
            {
                this->Stop();
            }

            this->m_NextFrame = (gpGlobals->time + 3.0f);
        }
    }
}