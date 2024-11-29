#include "precompiled.h"
#include "PugMod.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    gPugMod.SetState(STATE_DEAD);
}

void CPugMod::ServerDeactivate()
{
    /**/
}

int CPugMod::GetState()
{
    return this->m_State;
}

int CPugMod::SetState(int State)
{
    this->m_State = State;

    switch (this->m_State)
    {
        case STATE_DEAD:
        {
            gPugTask.Set(TASK_CHANGE_STATE, 2.0f,  false, gPugCvar.m_DM_Enable->value ? STATE_DEATHMATCH : STATE_WARMUP);
            break;
        }
        case STATE_WARMUP:
        case STATE_DEATHMATCH:
        {
            gPugTimer.Init();

            gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 O Modo ^3%s^1 está ativo até o início da partida.", gPugCvar.m_Tag->string, this->m_Data[State].Name);
            break;
        }
    }

    if (this->m_Data[State].Config)
    {
        gPugUtil.ServerCommand("exec addons/pugmod/cfg/%s.cfg", this->m_Data[State].Config);
    }

    return this->m_State;
}

void CPugMod::Status(edict_t *pEntity)
{
    auto Players = gPugUtil.GetPlayers(true, true);
    
    if (!FNullEnt(pEntity))
    {
        gPugUtil.SayText(pEntity, PRINT_TEAM_DEFAULT, "^4[%s]^1 Status: %s", gPugCvar.m_Tag->string, this->m_Data[this->m_State].Name);
        gPugUtil.SayText(pEntity, PRINT_TEAM_DEFAULT, "^4[%s]^1 Players: %d (Mínimo: %d, Máximo: %d)", gPugCvar.m_Tag->string, Players.size(), static_cast<int>(gPugCvar.m_PlayersMin->value), static_cast<int>(gPugCvar.m_PlayersMax->value));
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Status: %s", gPugCvar.m_Tag->string, this->m_Data[this->m_State].Name);
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Players: %d (Mínimo: %d, Máximo: %d)", gPugCvar.m_Tag->string, Players.size(), static_cast<int>(gPugCvar.m_PlayersMin->value), static_cast<int>(gPugCvar.m_PlayersMax->value));
    }
}