#include "precompiled.h"

CPugServerCmd gPugServerCmd;

void CPugServerCmd::ServerActivate()
{
    for (auto const & Command : m_CommandList)
    {
        g_engfuncs.pfnAddServerCommand((char*)Command.Name, Command.Function);
    }
}

void CPugServerCmd::AddAdmin()
{
    if (g_engfuncs.pfnCmd_Argc() >= 4)
    {
        gPugAdmin.Set(g_engfuncs.pfnCmd_Argv(1), g_engfuncs.pfnCmd_Argv(2), g_engfuncs.pfnCmd_Argv(3));
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: %s <steam> <nome> <flags>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
    }
}

void CPugServerCmd::AddClientCmd()
{
    if (g_engfuncs.pfnCmd_Argc() >= 4)
    {
        gPugClientCmd.Set(g_engfuncs.pfnCmd_Argv(1), g_engfuncs.pfnCmd_Argv(2), g_engfuncs.pfnCmd_Argv(3));
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: %s <index> <comando> <flags>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
    }
}

void CPugServerCmd::AddSpawn()
{
    vec3_t Vecs     = {0.0f, 0.0f, 0.0f};
    vec3_t Angles   = {0.0f, 0.0f, 0.0f};
    int Team        = 0;
    vec3_t VAngles  = {0.0f, 0.0f, 0.0f};

    if (g_engfuncs.pfnCmd_Argc() >= 7)
    {
        Vecs.x = std::stof(g_engfuncs.pfnCmd_Argv(1));
        Vecs.y = std::stof(g_engfuncs.pfnCmd_Argv(2));
        Vecs.z = std::stof(g_engfuncs.pfnCmd_Argv(3));

        Angles.x = std::stof(g_engfuncs.pfnCmd_Argv(4));
        Angles.y = std::stof(g_engfuncs.pfnCmd_Argv(5));
        Angles.z = std::stof(g_engfuncs.pfnCmd_Argv(6));

        if (g_engfuncs.pfnCmd_Argc() >= 8)
        {
            Team = std::stoi(g_engfuncs.pfnCmd_Argv(7));
        }
        
        if (g_engfuncs.pfnCmd_Argc() >= 11)
        {
            VAngles.x = std::stof(g_engfuncs.pfnCmd_Argv(8));
            VAngles.y = std::stof(g_engfuncs.pfnCmd_Argv(9));
            VAngles.z = std::stof(g_engfuncs.pfnCmd_Argv(10));
        }

        gPugDM.SetSpawn(Vecs, Angles, Team, VAngles);
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: %s <vec.x> <vec.y> <vec.z> <angle.x> <angle.y> <angle.z> [team] [vAngle.x] [vAngle.y] [vAngle.z]", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
    }
}

void CPugServerCmd::AddWeapon()
{
    if (g_engfuncs.pfnCmd_Argc() >= 4)
    {
        gPugDM.SetWeapon
        (
            g_engfuncs.pfnCmd_Argv(1),
            g_engfuncs.pfnCmd_Argv(2),
            std::stoi(g_engfuncs.pfnCmd_Argv(3))
        );
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: %s <nome> <texto> <bot>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
    }
}

void CPugServerCmd::AddMap()
{
    if (g_engfuncs.pfnCmd_Argc() >= 2)
    {
        gPugVoteMap.AddMap(g_engfuncs.pfnCmd_Argv(1));
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: %s <mapa>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
    }
}

void CPugServerCmd::Status()
{
    auto Players = gPugUtil.GetPlayers();
    auto State = gPugMod.GetState();

    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Status: %s", gPugCvar.m_Tag->string, g_Pug_String[State]);
    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Players %s: %d", gPugCvar.m_Tag->string,g_Pug_TeamId[TERRORIST], Players[TERRORIST].size());
    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Players %s: %d", gPugCvar.m_Tag->string,g_Pug_TeamId[CT], Players[CT].size());
    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Players %s: %d", gPugCvar.m_Tag->string,g_Pug_TeamId[SPECTATOR], Players[SPECTATOR].size());

    if (State >= STATE_FIRST_HALF && State <= STATE_END)
    {
        auto Score = gPugMod.GetScore();

        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Score %s: %d", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], Score[TERRORIST]);
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Score %s: %d", gPugCvar.m_Tag->string, g_Pug_TeamId[CT], Score[CT]);
    }
}
