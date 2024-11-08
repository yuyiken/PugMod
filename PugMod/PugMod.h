#pragma once

constexpr int STATE_DEAD = 0;
constexpr int STATE_DEATHMATCH = 1;
constexpr int STATE_END = 2;

class CPugMod
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void SetState(int State);

	void GetIntoGame(CBasePlayer* Player);
	bool JoinTeam(CBasePlayer* Player, int Slot);

	int m_State;
	int m_ScoreTR;
	int m_scoreCT;

	std::array<std::string, STATE_END + 1U> m_Config = { "pugmod.cfg", "deathmatch.cfg", "end.cfg"};
};

extern CPugMod gPugMod;
