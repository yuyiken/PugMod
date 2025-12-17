#include "precompiled.h"

CPugServerCmd gPugServerCmd;

void CPugServerCmd::ServerActivate()
{
    for (auto const & Command : m_CommandList)
    {
        ADD_SERVER_COMMAND((char*)Command.Name, Command.Function);
    }
}

void CPugServerCmd::AddAdmin()
{
    if (CMD_ARGC() >= 4)
    {
        gPugAdmin.Set(CMD_ARGV(1), CMD_ARGV(2), CMD_ARGV(3));
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <steam> <nome> <flags>", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::AddClientCmd()
{
    if (CMD_ARGC() >= 4)
    {
        gPugClientCmd.Set(CMD_ARGV(1), CMD_ARGV(2), CMD_ARGV(3));
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <index> <comando> <flags>", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::AddSpawn()
{
    vec3_t Vecs     = {0.0f, 0.0f, 0.0f};
    vec3_t Angles   = {0.0f, 0.0f, 0.0f};
    int Team        = 0;
    vec3_t VAngles  = {0.0f, 0.0f, 0.0f};

    if (CMD_ARGC() >= 7)
    {
        Vecs.x = std::stof(CMD_ARGV(1));
        Vecs.y = std::stof(CMD_ARGV(2));
        Vecs.z = std::stof(CMD_ARGV(3));

        Angles.x = std::stof(CMD_ARGV(4));
        Angles.y = std::stof(CMD_ARGV(5));
        Angles.z = std::stof(CMD_ARGV(6));

        if (CMD_ARGC() >= 8)
        {
            Team = std::stoi(CMD_ARGV(7));
        }
        
        if (CMD_ARGC() >= 11)
        {
            VAngles.x = std::stof(CMD_ARGV(8));
            VAngles.y = std::stof(CMD_ARGV(9));
            VAngles.z = std::stof(CMD_ARGV(10));
        }

        gPugDM.SetSpawn(Vecs, Angles, Team, VAngles);
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <vec.x> <vec.y> <vec.z> <angle.x> <angle.y> <angle.z> [team] [vAngle.x] [vAngle.y] [vAngle.z]", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::AddWeapon()
{
    if (CMD_ARGC() >= 4)
    {
        gPugDM.SetWeapon(CMD_ARGV(1), CMD_ARGV(2), std::stoi(CMD_ARGV(3)));
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <nome> <texto> <bot>", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::AddCvar()
{
    if (CMD_ARGC() >= 3)
    {
        gPugCvarControl.Add
        (
            CMD_ARGV(1),
            CMD_ARGV(2)
        );
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <nome> <valor>", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::AddMap()
{
    if (CMD_ARGC() >= 2)
    {
        gPugMapList.Add(CMD_ARGV(1));
    }
    else
    {
        LOG_CONSOLE(PLID, "[%s] Uso: %s <mapa>", Plugin_info.logtag, CMD_ARGV(0));
    }
}

void CPugServerCmd::Status()
{
    auto Players = gPugUtil.GetPlayers();

    auto State = gPugMod.GetState();

    LOG_CONSOLE
    (
        PLID, 
        "[%s] Status: %s (%s)", 
        gPugCvar.m_Tag->string, 
        gPugMod.GetString(State), 
        STRING(gpGlobals->mapname)
    );

    LOG_CONSOLE
    (
        PLID,
        "[%s] Players: (%s %d) x (%s %d) x (%s %d)",
        gPugCvar.m_Tag->string, 
        g_Pug_TeamName[TERRORIST],
        Players[TERRORIST].size(),
        g_Pug_TeamName[CT],
        Players[CT].size(),
        g_Pug_TeamName[SPECTATOR],
        Players[SPECTATOR].size()
    );

    if (State >= STATE_FIRST_HALF && State <= STATE_END)
    {
        LOG_CONSOLE
        (
            PLID, 
            "[%s] Scores: (%s %d) x (%s %d)", 
            gPugCvar.m_Tag->string, 
            g_Pug_TeamName[TERRORIST], 
            gPugMod.GetScore(TERRORIST),
            g_Pug_TeamName[CT], 
            gPugMod.GetScore(CT)
        );
    }
}
