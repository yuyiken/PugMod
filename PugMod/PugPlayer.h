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
	int TeamIndex;

	bool DM_HideMenu;
	std::map<int, int> DM_WeaponState;
	std::map<int, WeaponSlotInfo*> DM_LastWeapon;
} P_PLAYER, *LP_PLAYER;

class CPugPlayer
{
public:
	LP_PLAYER Get(const char* Auth);
	LP_PLAYER Get(edict_t* pEntity);
	LP_PLAYER Get(int EntityIndex);

	void Connect(edict_t* pEntity, const char* pszName, const char* pszAddress);
	void PutInServer(edict_t* pEntity);
	void GetIntoGame(CBasePlayer* Player);
	void JoinTeam(CBasePlayer* Player);

private:
	std::map<std::string, P_PLAYER> m_Players;
};

extern CPugPlayer gPugPlayer;