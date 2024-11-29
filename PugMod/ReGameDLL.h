#pragma once

extern IReGameApi *g_ReGameApi;
extern const ReGameFuncs_t *g_ReGameFuncs;
extern IReGameHookchains *g_ReGameHookchains;
extern CGameRules *g_pGameRules;

extern bool ReGameDLL_Init();
extern bool ReGameDLL_Stop();

extern CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain);
extern BOOL ReGameDLL_HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam *chain, CBasePlayer *Player, int Slot);
extern bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame *chain, CBasePlayer *Player);
extern void ReGameDLL_CBasePlayer_UpdateClientData(IReGameHook_CBasePlayer_UpdateClientData *chain, CBasePlayer *Player);
extern void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange);
extern bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem *chain, CBasePlayer *Player, ItemID ItemIndex, ItemRestType RestType);
extern edict_t *ReGameDLL_CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot *chain, CBasePlayer *Player);
extern void ReGameDLL_CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn *chain, CBasePlayer *Player);
extern void ReGameDLL_CBasePlayer_GiveDefaultItems(IReGameHook_CBasePlayer_GiveDefaultItems *chain, CBasePlayer *Player);
extern BOOL ReGameDLL_CSGameRules_FPlayerCanTakeDamage(IReGameHook_CSGameRules_FPlayerCanTakeDamage *chain,  CBasePlayer *Player, CBaseEntity *Entity);
extern BOOL ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage* chain, CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
extern void ReGameDLL_CSGameRules_PlayerKilled(IReGameHook_CSGameRules_PlayerKilled* chain, CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor);
extern void ReGameDLL_CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage* chain, CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
extern void ReGameDLL_CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* Player, PLAYER_ANIM playerAnimation);
extern void ReGameDLL_ShowVGUIMenu(IReGameHook_ShowVGUIMenu *chain, CBasePlayer *Player, int MenuType, int BitMask, char *szOldMenu);