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

										this->m_Hookchains->CBasePlayer_GetIntoGame()->registerHook(this->CBasePlayer_GetIntoGame);

										this->m_Hookchains->CSGameRules_PlayerSpawn()->registerHook(this->CSGameRules_PlayerSpawn);

										this->m_Hookchains->CBasePlayer_SetAnimation()->registerHook(this->CBasePlayer_SetAnimation);

										this->m_Hookchains->HandleMenu_ChooseTeam()->registerHook(this->HandleMenu_ChooseTeam);

										this->m_Hookchains->CBasePlayer_SwitchTeam()->registerHook(this->CBasePlayer_SwitchTeam);

										this->m_Hookchains->CSGameRules_SendDeathMessage()->registerHook(this->CSGameRules_SendDeathMessage);
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

		this->m_Hookchains->CSGameRules_GetPlayerSpawnSpot()->unregisterHook(this->CSGameRules_GetPlayerSpawnSpot);

		this->m_Hookchains->CBasePlayer_GetIntoGame()->unregisterHook(this->CBasePlayer_GetIntoGame);

		this->m_Hookchains->CSGameRules_PlayerSpawn()->unregisterHook(this->CSGameRules_PlayerSpawn);

		this->m_Hookchains->CBasePlayer_SetAnimation()->unregisterHook(this->CBasePlayer_SetAnimation);

		this->m_Hookchains->HandleMenu_ChooseTeam()->unregisterHook(this->HandleMenu_ChooseTeam);

		this->m_Hookchains->CBasePlayer_SwitchTeam()->unregisterHook(this->CBasePlayer_SwitchTeam);

		this->m_Hookchains->CSGameRules_SendDeathMessage()->unregisterHook(this->CSGameRules_SendDeathMessage);
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
	if (gPugDeathmatch.AddAccount(Player, Amount, Type, TrackChange))
	{
		Amount = 0;
	}

	chain->callNext(Player, Amount, Type, TrackChange);
}

bool CReGameDLL::CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType)
{
	if (gPugDeathmatch.HasRestrictItem(Player, ItemIndex, RestType))
	{
		return true;
	}

	return chain->callNext(Player, ItemIndex, RestType);
}

edict_t* CReGameDLL::CSGameRules_GetPlayerSpawnSpot(IReGameHook_CSGameRules_GetPlayerSpawnSpot* chain, CBasePlayer* Player)
{
	if (gPugDeathmatch.SetPosition(Player))
	{
		return nullptr;
	}

	return chain->callNext(Player);
}

bool CReGameDLL::CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player)
{
	auto Result = chain->callNext(Player);

	gPugPlayer.GetIntoGame(Player);

	gPugMod.GetIntoGame(Player);

	gPugDeathmatch.GetIntoGame(Player);

	return Result;
}

void CReGameDLL::CSGameRules_PlayerSpawn(IReGameHook_CSGameRules_PlayerSpawn* chain, CBasePlayer* Player)
{
	chain->callNext(Player);

	gPugDeathmatch.PlayerSpawn(Player);
}

void CReGameDLL::CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* Player, PLAYER_ANIM playerAnimation)
{
	chain->callNext(Player, playerAnimation);

	gPugDeathmatch.SetAnimation(Player, playerAnimation);
}

BOOL CReGameDLL::HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam* chain, CBasePlayer* Player, int Slot)
{
	if (gPugMod.JoinTeam(Player, Slot))
	{
		Slot = 0;
	}

	return chain->callNext(Player, Slot);
}

void CReGameDLL::CBasePlayer_SwitchTeam(IReGameHook_CBasePlayer_SwitchTeam* chain, CBasePlayer* Player)
{
	chain->callNext(Player);

	gPugPlayer.SwitchTeam(Player);
}

void CReGameDLL::CSGameRules_SendDeathMessage(IReGameHook_CSGameRules_SendDeathMessage* chain, CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (gPugDeathmatch.SendDeathMessage(Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill))
	{
		return;
	}

	chain->callNext(Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
}