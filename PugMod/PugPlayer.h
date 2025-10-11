#pragma once

typedef struct S_PLAYER_STATS
{
	float Frags;   // Frags
	int Deaths;	 // Deaths
	int Account; // Account
} P_PLAYER_STATS, *LP_PLAYER_STATS;

typedef struct S_PLAYER_INFO
{
	int EntityId;									 // Entity Index
	int UserId;										 // User Index
	std::string Auth;								 // Auth Id
	std::string Name;								 // Player Name
	int Flags;										 // Player Flags (Bitsum)
	int Status;										 // 0 Disconnected, 1 Connected, 2 In Game
	std::string DcReason;							 // Disconnect Reason
	TeamName Team;									 // Team
	std::array<P_PLAYER_STATS, STATE_END + 1> Stats; // Stats
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CPugPlayer
{
public:
	void ServerActivate();
    const char* GetPlayerAuthId(edict_t* pEntity);
	LP_PLAYER_INFO GetInfo(const char* AuthId);
	LP_PLAYER_INFO GetInfo(int EntityIndex);
	void PutInServer(edict_t* pEntity);
	void GetIntoGame(CBasePlayer* Player);
	void SwitchTeam(CBasePlayer* Player);
	void AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	void Disconnect(edict_t* pEntity, bool crash, const char* Reason);
private:
	std::map<std::string, P_PLAYER_INFO> m_Player;
};

extern CPugPlayer gPugPlayer;