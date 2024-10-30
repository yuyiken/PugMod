#pragma once

class CPugConfig
{
public:
	void ServerActivate();

	cvar_t* m_LogTag;
	cvar_t* m_PlayersMin;
	cvar_t* m_PlayersMax;
	cvar_t* m_Rounds;
	cvar_t* m_RoundsOT;
	cvar_t* m_OvertimeType;
	cvar_t* m_TeamType;
	std::array<cvar_t*, STATE_END + 1> m_Config;
};

extern CPugConfig gPugConfig;