#pragma once

constexpr auto SPAWN_POINT_MIN_DISTANCE = 250.0f;

typedef struct S_DM_SPAWN
{
	vec3_t Vecs;
	vec3_t Angles;
	int Team;
	vec3_t VAngles;
} P_DM_SPAWN, *LP_DM_SPAWN;

typedef struct S_DM_ITEM
{
	std::string Alias;
	std::string Label;
	int Enable;
	int Bot;
	int Slot;
} P_DM_ITEM, *LP_DM_ITEM;

typedef struct S_DM_INFO
{
	bool EquipMenu;
	std::map<int, int> State;
	std::map<int, WeaponSlotInfo*> Last;

	int HideKillFeed;
	int HitIndicator;
	int HSOnlyMode;
	int HudKDRatio;
	int KillFade;
	int KillSound;
	int MoneyFrag;

	void Reset()
	{
		this->EquipMenu = true;
		this->State.clear();
		this->Last.clear();
		
		this->HideKillFeed = (int)(gPugCvar.m_DM_HideKillFeed->value);
		this->HitIndicator = (int)(gPugCvar.m_DM_HitIndicator->value);
		this->HSOnlyMode = (int)(gPugCvar.m_DM_HSOnlyMode->value);
		this->HudKDRatio = (int)(gPugCvar.m_DM_HudKDRatio->value);
		this->KillFade = (int)(gPugCvar.m_DM_KillFade->value);
		this->KillSound = (int)(gPugCvar.m_DM_KillSound->value);
		this->MoneyFrag = (int)(gPugCvar.m_DM_MoneyFrag->value);
	}
} P_DM_INFO, *LP_DM_INFO;

class CPugDeathmatch
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void LoadSpawns();
	void LoadItems();

	void Init();
	void Stop();

	bool CheckDistance(CBasePlayer* Player, vec3_t Origin, float Distance);

	bool SetPosition(CBasePlayer* Player);
	bool AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	bool HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);
	void PlayerSpawn(CBasePlayer* Player);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnimation);
	bool SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);

	void GetIntoGame(CBasePlayer* Player);

	void EquipItem(CBasePlayer* Player, WeaponSlotInfo* Item);
	void EquipRandom(CBasePlayer* Player, int Slot);
	void EquipLast(CBasePlayer* Player);

	bool SetEquipMenu(CBasePlayer* Player, bool EquipMenu);

	void EquipMenu(int EntityIndex);
	static void EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void WeaponMenu(int EntityIndex, int Slot);
	static void WeaponMenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void OptionMenu(int EntityIndex);
	static void OptionMenuHandle(int EntityIndex, P_MENU_ITEM Item);

private:
	bool m_Running;
	std::vector<P_DM_SPAWN> m_Spawns;
	std::vector<P_DM_ITEM> m_Items;
};

extern CPugDeathmatch gPugDeathmatch;
