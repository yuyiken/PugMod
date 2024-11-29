#include "precompiled.h"

CPugServerCmd gPugServerCmd;

void CPugServerCmd::ServerActivate()
{
    g_engfuncs.pfnAddServerCommand((char*)"pug_status", this->Status);
    g_engfuncs.pfnAddServerCommand((char*)"pug_add_weapon", this->AddWeapon);
}

void CPugServerCmd::Status()
{
    gPugMod.Status(nullptr);
}

void CPugServerCmd::AddWeapon()
{
    if (g_engfuncs.pfnCmd_Argc() >= 4)
    {
        gPugDeathmatch.AddWeapon(g_engfuncs.pfnCmd_Argv(1), g_engfuncs.pfnCmd_Argv(2), std::stoi(g_engfuncs.pfnCmd_Argv(3)) ? true : false);
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Uso: pug_add_weapon <alias> <nome> <bot>", Plugin_info.logtag);
    }
}