#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
	this->SetState(STATE_DEAD);
}

void CPugMod::ServerDeactivate()
{
	if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
	{
		this->SetState(STATE_END);
	}
}

void CPugMod::SetState(int State)
{
	this->m_State = State;

	switch (State)
	{
		case STATE_DEAD:
		{
			gPugTask.Create(TASK_CHANGE_STATE, 6.0f, false, STATE_WARMUP);
			break;
		}
		case STATE_WARMUP:
		{
			gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 Iniciando Aquecimento! Prepare-se!!", Plugin_info.logtag);

			gPugWarmup.Init();
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

	if (gPugCvar.m_Config[State])
	{
		if (gPugCvar.m_Config[State]->string)
		{
			if (gPugCvar.m_Config[State]->string[0] != '\0')
			{
				gPugUtil.ServerCommand("exec addons/pugmod/cfg/%s", gPugCvar.m_Config[this->m_State]->string);
			}
		}
	}
}