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
	static edict_t* CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player);
};

extern CReGameDLL gReGameDLL;
