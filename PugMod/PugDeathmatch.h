#pragma once

constexpr auto SPAWN_POINT_MIN_DISTANCE = 250.0f;

typedef struct S_SPAWN
{
	vec3_t Vecs;
	vec3_t Angles;
	int Team;
	vec3_t VAngles;
} P_SPAWN, *LP_SPAWN;

typedef struct S_WEAPON_INFO
{
	WeaponSlotInfo *SlotInfo;
	std::string Label;
	bool Bot;
} P_WEAPON_INFO, *LP_WEAPON_INFO;

typedef struct S_PLAYER_DM
{
    bool m_HideMenu;

    std::array<WeaponIdType, C4_SLOT + 1U> m_WeaponLast;
    std::map<int, int> m_WeaponState;

    int m_Headshots;

	short m_HideKillFeed;
    short m_HitIndicator;
	short m_HSOnlyMode;
	short m_HudKDRatio;
	short m_KillFade;
	short m_KillSound;
	short m_MoneyFrag;
    
    void Clear()
    {
        this->m_HideMenu = false;
        this->m_WeaponLast.fill(WEAPON_NONE);
        this->m_WeaponState.clear();
        this->m_Headshots = 0;
        this->m_HideKillFeed = 0;
        this->m_HitIndicator = 0;
        this->m_HSOnlyMode = 0;
        this->m_KillFade = 0;
        this->m_KillSound = 0;
        this->m_MoneyFrag = 0;
    }
} P_PLAYER_DM, *LP_PLAYER_DM;

constexpr hudtextparms_t g_DM_HudMain = {-1.0f, 0.85f, 2, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0.0f, 0.0f, 6.0f, 3.0f, 1};
constexpr hudtextparms_t g_DM_HudHeal = {-1.0f, 0.60f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 2.0f, 1.0f, 2};
constexpr hudtextparms_t g_DM_HudInfo = {-1.0f, 0.01f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 1.1f, 0.0f, 3};
constexpr hudtextparms_t g_DM_HudHits = {-1.0f, 0.55f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 0.75f, 0.35f, 4};

class CPugDeathmatch
{
public:
    void ServerActivate();

    bool GetPlayerSpawnSpot(CBasePlayer *Player);
    bool CheckDistance(CBasePlayer *Player, vec3_t Origin, float Distance);

	void AddWeapon(std::string Alias, std::string Label, bool Bot);

	void MenuOption(int EntityIndex);
	static void MenuOptionHandle(int EntityIndex, P_MENU_ITEM Item);

	void Menu(CBasePlayer *Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void MenuEquip(int EntityIndex, int Slot);
	static void MenuEquipHandle(int EntityIndex, P_MENU_ITEM Item);

	void EquipItem(int EntityIndex, WeaponIdType ItemIndex);
    bool EquipRandomItem(int EntityIndex, int Slot);
	
	void GetIntoGame(CBasePlayer *Player);
	bool FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity);
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void Killed(CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor);
	bool SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
	void UpdateClientData(CBasePlayer* Player);
	bool AddAccount(CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange);
	void SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation);
	bool DropItem(CBasePlayer *Player);
	bool ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *szOldMenu);

private:
    std::vector<P_SPAWN> m_Spawns;
    std::vector<P_WEAPON_INFO> m_Weapon;
};

extern CPugDeathmatch gPugDeathmatch;