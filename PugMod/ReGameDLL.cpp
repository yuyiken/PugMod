#include "precompiled.h"

CReGameDLL gReGameDLL;

bool CReGameDLL::Init()
{
	auto szGameDLLModule = gpMetaUtilFuncs->pfnGetGameInfo(&Plugin_info, GINFO_DLL_FULLPATH);

	if (szGameDLLModule)
	{
		auto GameModule = Sys_LoadModule(szGameDLLModule);

		if (GameModule)
		{
			auto ifaceFactory = Sys_GetFactory(GameModule);

			if (ifaceFactory)
			{
				int retCode = 0;

				this->m_API = (IReGameApi*)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

				if (this->m_API)
				{
					if (this->m_API->GetMajorVersion() == REGAMEDLL_API_VERSION_MAJOR)
					{
						if (this->m_API->GetMinorVersion() >= REGAMEDLL_API_VERSION_MINOR)
						{
							if (this->m_API->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
							{
								if (gReGameDLL.m_API->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
								{
									this->m_Funcs = this->m_API->GetFuncs();

									this->m_Hookchains = this->m_API->GetHookchains();

									if (this->m_Hookchains)
									{
										this->m_Hookchains->InstallGameRules()->registerHook(this->InstallGameRules);

										this->m_Hookchains->CBasePlayer_AddAccount()->registerHook(this->CBasePlayer_AddAccount);

										this->m_Hookchains->CBasePlayer_HasRestrictItem()->registerHook(this->CBasePlayer_HasRestrictItem);
									}
								}

								return true;
							}
						}
					}
				}
			}
		}
	}

	gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] ReGameDLL_CS API failed to load.", Plugin_info.logtag);

	return false;
}

bool CReGameDLL::Stop()
{
	if (this->m_Hookchains)
	{
		this->m_Hookchains->InstallGameRules()->unregisterHook(this->InstallGameRules);

		this->m_Hookchains->CBasePlayer_AddAccount()->unregisterHook(this->CBasePlayer_AddAccount);

		this->m_Hookchains->CBasePlayer_HasRestrictItem()->unregisterHook(this->CBasePlayer_HasRestrictItem);
	}
	
	return false;
}

CGameRules* CReGameDLL::InstallGameRules(IReGameHook_InstallGameRules* chain)
{
	gReGameDLL.m_Rules = chain->callNext();

	return gReGameDLL.m_Rules;
}

void CReGameDLL::CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int Amount, RewardType Type, bool TrackChange)
{
	if (gPugWarmup.AddAccount(Player, Amount, Type, TrackChange))
	{
		Amount = 0;
	}

	chain->callNext(Player, Amount, Type, TrackChange);
}

bool CReGameDLL::CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType)
{
	if (gPugWarmup.HasRestrictItem(Player, ItemIndex, RestType))
	{
		return true;
	}

	return chain->callNext(Player, ItemIndex, RestType);
}

//
//IReGameApi			*g_ReGameApi;
//const ReGameFuncs_t	*g_ReGameFuncs;
//IReGameHookchains	*g_ReGameHookchains;
//CGameRules			*g_pGameRules;
//
//bool ReGameDLL_Init()
//{
//	const char *szGameDLLModule = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_DLL_FULLPATH);
//
//	if (!szGameDLLModule)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Failed to get GameDLL module", Plugin_info.logtag);
//		return false;
//	}
//
//	CSysModule *gameModule = Sys_LoadModule(szGameDLLModule);
//
//	if (!gameModule)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Failed to locate GameDLL module", Plugin_info.logtag);
//		return false;
//	}
//
//	CreateInterfaceFn ifaceFactory = Sys_GetFactory(gameModule);
//
//	if (!ifaceFactory)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Failed to locate interface factory in GameDLL module", Plugin_info.logtag);
//		return false;
//	}
//
//	int retCode = 0;
//
//	g_ReGameApi = (IReGameApi *)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);
//
//	if (!g_ReGameApi)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Failed to locate retrieve rehlds api interface from GameDLL module, return code is %d", Plugin_info.logtag, retCode);
//		return false;
//	}
//
//	int majorVersion = g_ReGameApi->GetMajorVersion();
//	int minorVersion = g_ReGameApi->GetMinorVersion();
//
//	if (majorVersion != REGAMEDLL_API_VERSION_MAJOR)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API major version mismatch; expected %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, majorVersion);
//
//		if (majorVersion < REGAMEDLL_API_VERSION_MAJOR)
//		{
//			gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Please update the ReGameDLL up to a major version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR);
//		}
//		else if (majorVersion > REGAMEDLL_API_VERSION_MAJOR)
//		{
//			gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Please update the %s up to a major version API >= %d", Plugin_info.logtag, Plugin_info.logtag, majorVersion);
//		}
//
//		return false;
//	}
//
//	if (minorVersion < REGAMEDLL_API_VERSION_MINOR)
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] ReGameDLL API minor version mismatch; expected at least %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR, minorVersion);
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Please update the ReGameDLL up to a minor version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR);
//		return false;
//	}
//
//	g_ReGameFuncs = g_ReGameApi->GetFuncs();
//
//	g_ReGameHookchains = g_ReGameApi->GetHookchains();
//
//	if (!g_ReGameApi->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Interface CCSEntity API version '%s' not found", Plugin_info.logtag, CSENTITY_API_INTERFACE_VERSION);
//		return false;
//	}
//
//	g_ReGameHookchains->InstallGameRules()->registerHook(ReGameDLL_InstallGameRules);
//
//	g_ReGameHookchains->InternalCommand()->registerHook(ReGameDLL_InternalCommand);
//
//	g_ReGameHookchains->CBasePlayer_AddAccount()->registerHook(ReGameDLL_CBasePlayer_AddAccount);
//
//	g_ReGameHookchains->CBasePlayer_HasRestrictItem()->registerHook(ReGameDLL_CBasePlayer_HasRestrictItem);
//
//	g_ReGameHookchains->CSGameRules_GetPlayerSpawnSpot()->registerHook(this->CSGameRules_GetPlayerSpawnSpot);
//
//	return true;
//}
//
//bool ReGameDLL_Stop()
//{
//	g_ReGameHookchains->InstallGameRules()->unregisterHook(ReGameDLL_InstallGameRules);
//
//	g_ReGameHookchains->InternalCommand()->unregisterHook(ReGameDLL_InternalCommand);
//
//	g_ReGameHookchains->CBasePlayer_AddAccount()->unregisterHook(ReGameDLL_CBasePlayer_AddAccount);
//
//	g_ReGameHookchains->CBasePlayer_HasRestrictItem()->unregisterHook(ReGameDLL_CBasePlayer_HasRestrictItem);
//
//	g_ReGameHookchains->CSGameRules_GetPlayerSpawnSpot()->unregisterHook(this->CSGameRules_GetPlayerSpawnSpot);
//
//	return true;
//}
//
//CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain)
//{
//	auto gamerules = chain->callNext();
//
//	if (!g_ReGameApi->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
//	{
//		gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Interface GameRules API version '%s' not found", Plugin_info.logtag, GAMERULES_API_INTERFACE_VERSION);
//	}
//	else
//	{
//		g_pGameRules = gamerules;
//	}
//	
//	return gamerules;
//}
//
//void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1)
//{
//	chain->callNext(pEntity, pcmd, parg1);
//}
//
//void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount* chain, CBasePlayer* Player, int Amount, RewardType Type, bool TrackChange)
//{
//	if (gPugWarmup.AddAccount(Player, Amount, Type, TrackChange))
//	{
//		Amount = 0;
//	}
//
//	chain->callNext(Player, Amount, Type, TrackChange);
//}
//
//bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType)
//{
//	if (gPugWarmup.HasRestrictItem(Player, ItemIndex, RestType))
//	{
//		return true;
//	}
//
//	return chain->callNext(Player, ItemIndex, RestType);
//}