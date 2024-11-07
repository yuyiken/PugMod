#pragma once

typedef struct S_PLAYER
{
	int EntityIndex;
	int UserIndex;
	edict_t* Entity;

	std::string Name;
	std::string Address;
	std::string Auth;

	time_t ConnectTime;
	int Status; // 0 Disconnected, 1 Connecting, 2 In Server, 3 InGame
	TeamName TeamIndex;

	P_DM_INFO DeathMatch;
} P_PLAYER, *LP_PLAYER;

class CPugPlayer
{
public:
	void ServerActivate();

	const char* GetAuthId(edict_t* pEntity);

	LP_PLAYER Get(const char* Auth);
	LP_PLAYER Get(edict_t* pEntity);
	LP_PLAYER Get(int EntityIndex);
	LP_PLAYER Get(CBasePlayer* Player);

	void Connect(edict_t* pEntity, const char* pszName, const char* pszAddress);
	void PutInServer(edict_t* pEntity);
	void GetIntoGame(CBasePlayer* Player);
	void JoinTeam(CBasePlayer* Player);
	void SwitchTeam(CBasePlayer* Player);

private:
	std::map<std::string, P_PLAYER> m_Players;
};

extern CPugPlayer gPugPlayer;