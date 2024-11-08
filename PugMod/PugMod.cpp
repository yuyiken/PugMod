#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
	this->SetState(STATE_DEAD);
}

void CPugMod::ServerDeactivate()
{
	//if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
	//{
	//	this->SetState(STATE_END);
	//}
}

void CPugMod::SetState(int State)
{
	this->m_State = State;

	switch (State)
	{
		case STATE_DEAD:
		{
			gPugTask.Create(TASK_CHANGE_STATE, 5.0, false, STATE_DEATHMATCH);
			break;
		}
		case STATE_DEATHMATCH:
		{
			gPugDeathmatch.Init();
			break;
		}
	}

	if (!this->m_Config[State].empty())
	{
		gPugUtil.ServerCommand("exec addons/pugmod/cfg/%s", this->m_Config[State].c_str());
	}
}

void CPugMod::GetIntoGame(CBasePlayer* Player)
{
	if (this->m_State != STATE_DEAD)
	{
		if (!Player->IsBot())
		{
			gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 %s Build %s (^3%s^1)", Plugin_info.logtag, Plugin_info.name, Plugin_info.date, Plugin_info.author);
			gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Digite ^4.help^1 para lista de comandos.", Plugin_info.logtag);
		}
	}
}

bool CPugMod::JoinTeam(CBasePlayer* Player, int Slot)
{
	if (Player->m_iTeam == UNASSIGNED)
	{
		if (!Player->IsBot())
		{
			gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
			gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
		}
	}

	return false;
}
