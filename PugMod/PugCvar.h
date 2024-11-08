#pragma once
class CPugCvar
{
public:
	void ServerActivate();
	cvar_t* Register(const char* Name, const char* Value);

	cvar_t* m_SvRestart;

	cvar_t* m_LogTag;
	cvar_t* m_PlayersMin;
	cvar_t* m_PlayersMax;
	cvar_t* m_Rounds;
	cvar_t* m_RoundsOT;
	cvar_t* m_OvertimeType;
	cvar_t* m_TeamType;

	cvar_t* m_DM_Enable;
	cvar_t* m_DM_HideKillFeed;
	cvar_t* m_DM_HitIndicator;
	cvar_t* m_DM_HSOnlyMode;
	cvar_t* m_DM_HudKDRatio;
	cvar_t* m_DM_KillFade;
	cvar_t* m_DM_KillHP;
	cvar_t* m_DM_KillHPHS;
	cvar_t* m_DM_KillRepairArmor;
	cvar_t* m_DM_KillHealedMsg;
	cvar_t* m_DM_KillSound;
	cvar_t* m_DM_MoneyFrag;

public:
	std::map<std::string, cvar_t> m_Data;
};

extern CPugCvar gPugCvar;