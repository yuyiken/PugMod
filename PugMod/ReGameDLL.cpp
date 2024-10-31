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

										this->m_Hookchains->CSGameRules_GetPlayerSpawnSpot()->registerHook(this->CSGameRules_GetPlayerSpawnSpot);
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

		this->m_Hookchains->CSGameRules_GetPlayerSpawnSpot()->registerHook(this->CSGameRules_GetPlayerSpawnSpot);
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

edict_t* CReGameDLL::CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player)
{
	if (gPugSpawn.SetPlayerPosition(Player))
	{
		return nullptr;
	}

	return chain->callNext(Player);
}