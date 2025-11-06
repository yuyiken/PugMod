#pragma once

class CPugStats
{
public:
	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay);
	
    void GetIntoGame(CBasePlayer *Player);
    void DropClient(edict_t *pEntity);
	void TakeDamage(CBasePlayer* Victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	bool ShowHP(CBasePlayer* Player, bool InConsole);
	bool ShowDamage(CBasePlayer* Player, bool InConsole);
	bool ShowReceivedDamage(CBasePlayer* Player, bool InConsole);
	bool ShowSummary(CBasePlayer* Player, bool InConsole);

private:
	std::array<std::array<int, MAX_CLIENTS + 1>, MAX_CLIENTS + 1> m_RoundDmg;
	std::array<std::array<int, MAX_CLIENTS + 1>, MAX_CLIENTS + 1> m_RoundHit;
};

extern CPugStats gPugStats;