#pragma once

constexpr auto STATE_DEAD = 0;
constexpr auto STATE_WARMUP = 1;
constexpr auto STATE_START = 2;
constexpr auto STATE_FIRST_HALF = 3;
constexpr auto STATE_HALFTIME = 4;
constexpr auto STATE_SECOND_HALF = 5;
constexpr auto STATE_OVERTIME = 6;
constexpr auto STATE_END = 7;

class CPugMod
{
public:
	void ServerActivate();
	void ServerDeactivate();

private:
	cvar_t* m_PlayersMin;
	cvar_t* m_PlayersMax;
	cvar_t* m_Rounds;
	cvar_t* m_RoundsOT;
	cvar_t* m_OvertimeType;
	cvar_t* m_TeamType;
};

extern CPugMod gPugMod;
