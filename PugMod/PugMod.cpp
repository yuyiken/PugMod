#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Iniciando Pug Mod", __func__);

	this->SetState(STATE_DEAD);
}

void CPugMod::ServerDeactivate()
{
	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Desativando Pug Mod", __func__);
}

void CPugMod::SetState(int State)
{
	this->m_State = State;

	gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Inciando estado: %s", __func__, PUG_MOD_STATES_STR[State]);

	switch (State)
	{
		case STATE_DEAD:
		{
			gPugTask.Create(1337, 6.0, false);
			break;
		}
		case STATE_WARMUP:
		{
			gPugUtil.HudMessage(NULL, gPugUtil.HudParam(0, 255, 255, -1.0, 0.2, 1, 10.0, 10.0, 0.5f, 0.5f, 4), "Aquecimento Inciado!^nPrepare-se!!");

			gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 ^3Aquecimento Iniciado!!^1 Prepare-se!!", Plugin_info.logtag);

			if (g_pGameRules)
			{
				CSGameRules()->RestartRound();
			}

			break;
		}
		case STATE_START:
		{
			break;
		}
		case STATE_FIRST_HALF:
		{
			break;
		}
		case STATE_HALFTIME:
		{
			break;
		}
		case STATE_SECOND_HALF:
		{
			break;
		}
		case STATE_OVERTIME:
		{
			break;
		}
		case STATE_END:
		{
			break;
		}
	}

	if (gPugConfig.m_Config[State])
	{
		if (gPugConfig.m_Config[State]->string)
		{
			if (gPugConfig.m_Config[State]->string[0] != '\0')
			{
				gPugUtil.ServerCommand("exec addons/pugmod/cfg/%s", gPugConfig.m_Config[this->m_State]->string);
			}
		}
	}
}