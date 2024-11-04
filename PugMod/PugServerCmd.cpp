#include "precompiled.h"

CPugServerCmd gPugServerCmd;

void CPugServerCmd::ServerActivate()
{
	g_engfuncs.pfnAddServerCommand("pug_dm_add_spawn", this->CSDM_AddSpawn);

	g_engfuncs.pfnAddServerCommand("pug_dm_add_item", this->CSDM_AddItem);
}

void CPugServerCmd::CSDM_AddSpawn()
{
	if (g_engfuncs.pfnCmd_Argc() == 11)
	{
		// gPugDeathmatch.AddSpawn();
		return;
	}

	gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Uso: pug_dm_add_spawn <?> <?> <?> <?> <?> <?> <?> <?> <?> <?>", Plugin_info.logtag);
}

void CPugServerCmd::CSDM_AddItem()
{
	if (g_engfuncs.pfnCmd_Argc() == 6)
	{
		gPugDeathmatch.AddItem
		(
			g_engfuncs.pfnCmd_Argv(1),
			g_engfuncs.pfnCmd_Argv(2),
			std::stoi(g_engfuncs.pfnCmd_Argv(3)),
			std::stoi(g_engfuncs.pfnCmd_Argv(4))
		);

		return;
	}

	gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Uso: pug_dm_add_weapon <nome> <texto> <ativo> <bot>", Plugin_info.logtag);
}