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

typedef struct S_PLAYER_INFO
{
	bool HideMenu;
	std::map<int, int> WeaponState;
	std::map<int, int> WeaponAnim;

	WeaponSlotInfo* LastPrimary;
	WeaponSlotInfo* LastSecondary;

	void Reset()
	{
		this->HideMenu = false;
		this->WeaponState.clear();
		this->WeaponAnim.clear();
		this->LastPrimary = nullptr;
		this->LastSecondary = nullptr;
	}
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CPugDeathmatch
{
public:
	void ServerActivate();
	void ServerDeactivate();

	void LoadSpawns();
	void LoadItems();

	void Init();
	void Stop();

	bool AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	bool HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);
	bool SetPlayerPosition(CBasePlayer* Player);
	void PlayerSpawn(CBasePlayer* Player);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnimation);

	bool CheckDistance(CBasePlayer* Player, vec3_t Origin, float Distance);

	void ResetPlayer(CBasePlayer* Player);

	void EquipItem(CBasePlayer* Player, WeaponSlotInfo* Item);
	void EquipRandom(CBasePlayer* Player, int Slot);
	bool EquipLast(CBasePlayer* Player);

	bool SetHideMenu(CBasePlayer* Player, bool HideMenu);
	bool GetHideMenu(int EntityIndex);

	void SetWeaponState(int EntityIndex, CBasePlayerWeapon* Weapon);
	int GetWeaponState(int EntityIndex, int WeaponIndex);

	void EquipMenu(int EntityIndex);
	static void EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void WeaponMenu(int EntityIndex, int Slot);
	static void WeaponMenuHandle(int EntityIndex, P_MENU_ITEM Item);

private:
	bool m_Running;
	std::vector<P_DM_SPAWN> m_Spawns;
	std::vector<P_DM_ITEM> m_Items;
	std::map<int, P_PLAYER_INFO> m_Info;
};

extern CPugDeathmatch gPugDeathmatch;
