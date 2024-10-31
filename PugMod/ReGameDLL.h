#pragma once

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
};

extern CReGameDLL gReGameDLL;

//extern IReGameApi			*g_ReGameApi;
//extern const ReGameFuncs_t	*g_ReGameFuncs;
//extern IReGameHookchains	*g_ReGameHookchains;
//extern CGameRules			*g_pGameRules;
//
//extern bool ReGameDLL_Init();
//extern bool ReGameDLL_Stop();
//
//CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules* chain);
//void ReAPI_SV_DropClient(IRehldsHook_SV_DropClient* chain, IGameClient* client, bool crash, const char* Reason);
//void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1);
//void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int Amount, RewardType Type, bool TrackChange);
//bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);