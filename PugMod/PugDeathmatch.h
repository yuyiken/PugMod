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
	std::map<int, int> Option;

	//int HideKillFeed;
	//int HitIndicator;
	//int HSOnlyMode;
	//int HudKDRatio;
	//int KillFade;
	//int KillSound;
	//int MoneyFrag;

	void Reset()
	{
		this->EquipMenu = true;
		this->State.clear();
		this->Last.clear();
		this->Option.clear();

		this->Option[0] = (int)(gPugCvar.m_DM_HideKillFeed->value);
		this->Option[1] = (int)(gPugCvar.m_DM_HitIndicator->value);
		this->Option[2] = (int)(gPugCvar.m_DM_HSOnlyMode->value);
		this->Option[3] = (int)(gPugCvar.m_DM_HudKDRatio->value);
		this->Option[4] = (int)(gPugCvar.m_DM_KillFade->value);
		this->Option[5] = (int)(gPugCvar.m_DM_KillSound->value);
		this->Option[6] = (int)(gPugCvar.m_DM_MoneyFrag->value);
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

	void GetIntoGame(CBasePlayer* Player);
	bool SetPosition(CBasePlayer* Player);
	bool AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	bool HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);
	void PlayerSpawn(CBasePlayer* Player);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnimation);
	bool SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void PlayerKilled(CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor);

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

	const std::array<std::string, 9U> m_HitGroup = { "Acerto", "Cabeça", "Peito", "Estômago", "Braço Esquerdo", "Braço Direito", "Perna Esquerda", "Perna Direita", "Escudo" };
	const std::array<std::string, 7U> m_Option = { "Ocultar Kill Feed", "Indicador de acerto", "Modo Headshot", "Exibir Stats", "Piscar Tela ao matar", "Som ao matar", "Frags no dinheiro" };
};

extern CPugDeathmatch gPugDeathmatch;
