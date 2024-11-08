#pragma once

class CReGameDLL
{
public:
	bool Init();
	bool Stop();

	IReGameApi* m_API;
	const ReGameFuncs_t* m_Funcs;
	IReGameHookchains* m_Hookchains;
	CGameRules* m_Rules;

	static CGameRules* InstallGameRules(IReGameHook_InstallGameRules* chain);
	static void CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int Amount, RewardType Type, bool TrackChange);
	static bool CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);
	static edict_t* CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player);
	static bool CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player);
	static void CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn* chain, CBasePlayer* Player);
	static void CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* Player, PLAYER_ANIM playerAnimation);
	static BOOL HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam* chain, CBasePlayer* Player, int Slot);
	static void CBasePlayer_SwitchTeam(IReGameHook_CBasePlayer_SwitchTeam* chain, CBasePlayer* Player);
	static void CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage* chain, CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
	static BOOL CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage* chain, CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	static void CSGameRules_PlayerKilled(IReGameHook_CSGameRules_PlayerKilled* chain, CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor);
};

extern CReGameDLL gReGameDLL;
