#pragma once

typedef struct S_PLAYER_INFO
{
	int EntityIndex;					// Entity Index
	int UserId;							// User Index
	std::string Auth;					// Auth Id
	std::string Name;					// Player Name
	std::string Address;				// IP Address
	std::string Flags;					// Player Flags (String)
	int AdminFlags;						// Player Flags (Bitsum)
	int Status;							// 0 Disconnected, 1 Connected, 2 In Game
	std::string DcReason;				// Last Disconnect reason
	TeamName LastTeam;					// Last Team
	std::array<int, STATE_END + 1U> Frags;	// Frags by state
	std::array<int, STATE_END + 1U> Deaths;	// Deaths by state
	int Money;							// Last Money
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CPugPlayer
{
public:
	void ServerActivate();
    const char* GetPlayerAuthId(edict_t* pEntity);
	LP_PLAYER_INFO GetInfo(const char* AuthId);
	LP_PLAYER_INFO GetInfo(int UserIndex);
	bool Connect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
	void GetIntoGame(CBasePlayer* Player);
	void SwitchTeam(CBasePlayer* Player);
	void AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	void Disconnect(edict_t* pEntity, bool crash, const char* Reason);
	// void PlayerMenu(CBasePlayer* Player);
	// void PlayerMenuHandle(int EntityIndex, P_MENU_ITEM Item);
	// void ShowInfo(CBasePlayer* Player, int UserIndex);
	// void PlayerMenuActionHandle(CBasePlayer* Player, P_MENU_ITEM Item);
	// void PlayerBanMenuActionHandle(CBasePlayer* Player, P_MENU_ITEM Item);
private:
	std::map<std::string, P_PLAYER_INFO> m_Player;
};

extern CPugPlayer gPugPlayer;