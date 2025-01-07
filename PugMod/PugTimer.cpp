#include "precompiled.h"

CPugtimer gPugTimer;

void CPugtimer::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;
}

void CPugtimer::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;
}

void CPugtimer::Init()
{
    this->m_Run = true;

    this->m_NextFrame = 0.0f;

    this->m_Time = time(nullptr);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A partida vai começar quando todos estiverem em jogo.", gPugCvar.m_Tag->string);
}

void CPugtimer::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_Time = 0;

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Todos os jogadores estão nos times e prontos!", gPugCvar.m_Tag->string);

        gPugMod.NextState();
    }
}

void CPugtimer::RestartRound()
{
    if (this->m_Run)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_bCompleteReset)
            {
                this->Init();
            }
        }
    }
}

void CPugtimer::StartFrame()
{
    if (this->m_Run)
    {
        if (g_pGameRules)
        {
            if (gpGlobals->time >= this->m_NextFrame)
            {
                auto Needed = (static_cast<int>(gPugCvar.m_PlayersMin->value) - (CSGameRules()->m_iNumTerrorist + CSGameRules()->m_iNumCT));
                
                if (Needed > 0)
                {
                    this->m_Time = time(nullptr);

                    if (Needed > 1)
                    {
                        gPugUtil.SendHud(nullptr, g_Timer_HudParam, "AQUECIMENTO\n%d JOGADORES NECESSÁRIOS", Needed);
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_Timer_HudParam, "AQUECIMENTO\n%d JOGADOR NECESSÁRIO", Needed);
                    }
                }
                else
                {
                    time_t RemainTime = (time_t)((time_t)(gPugCvar.m_ReadyTimeLimit->value) - (time(nullptr) - this->m_Time));

                    if (RemainTime > 0)
                    {
                        char szTime[32] = { 0 };

                        strftime(szTime, sizeof(szTime), "INICIANDO PARTIDA EM\n%M:%S", localtime(&RemainTime));

                        gPugUtil.SendHud(nullptr, g_Timer_HudParam, szTime);
                    }
                    else
                    {
                        this->Stop();
                    }
                }

                this->m_NextFrame = (gpGlobals->time + 0.9f);
            }
        }
    }
}