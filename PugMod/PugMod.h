#pragma once

constexpr int STATE_DEAD = 0;
constexpr int STATE_DEATHMATCH = 1;
constexpr int STATE_END = 2;

const char PUG_MOD_STATES_STR[][16] =
{
	"Morto",
	"Deathmatch",
	"Fim"
};

class CPugMod
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void SetState(int State);

	void GetIntoGame(CBasePlayer* Player);
	void JoinTeam(CBasePlayer* Player);

	int m_State;
	int m_ScoreTR;
	int m_scoreCT;
};

extern CPugMod gPugMod;
