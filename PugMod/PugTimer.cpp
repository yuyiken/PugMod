#include "precompiled.h"

CPugtimer gPugTimer;

void CPugtimer::Init()
{
    this->m_Time = time(nullptr);

    gPugTask.Set(TASK_PUG_TIMER, 0.5f, true, 0);

    gPugUtil.SayText(nullptr, PRINT_TEAM_RED, "^4[%s]^1 A partida vai começar quando todos estiverem em jogo.", gPugCvar.m_Tag->string);
}

void CPugtimer::Stop()
{
    gPugTask.Remove(TASK_PUG_TIMER);

    gPugUtil.SayText(nullptr, PRINT_TEAM_RED, "^4[%s]^1 Todos os jogadores estão nos times e prontos!", gPugCvar.m_Tag->string);

    // Set New Pug State
}

void CPugtimer::Task()
{
    auto NeededCount = (static_cast<int>(gPugCvar.m_PlayersMin->value) - gPugUtil.GetPlayers(true, true).size());

    if (NeededCount > 0)
    {
        this->m_Time = time(nullptr);

        if (NeededCount > 1)
        {
            gPugUtil.SendHud(nullptr, g_WARMUP_HudTimer, "AQUECIMENTO\n%d JOGADORES NECESSÁRIOS", NeededCount);
        }
        else
        {
            gPugUtil.SendHud(nullptr, g_WARMUP_HudTimer, "AQUECIMENTO\n%d JOGADOR NECESSÁRIO", NeededCount);
        }
    }
    else
    {
        time_t RemainTime = (time_t)((time_t)(gPugCvar.m_WarmupTimeLimit->value) - (time(nullptr) - this->m_Time));

        if (RemainTime > 0)
        {
            struct tm *tm_info = localtime(&RemainTime);

            if (tm_info)
            {
                char Time[32] = { 0 };

                strftime(Time, sizeof(Time), "INICIANDO PARTIDA EM\n%M:%S", tm_info);

                gPugUtil.SendHud(nullptr, g_WARMUP_HudTimer, Time);
            }
        }
        else
        {
            this->Stop();
        }
    }
}