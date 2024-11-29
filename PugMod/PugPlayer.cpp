#include "precompiled.h"
#include "PugPlayer.h"

CPugPlayer gPugPlayer;

LP_PLAYER_INFO CPugPlayer::GetInfo(int EntityIndex)
{
    if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
    {
        auto pEntity = INDEXENT(EntityIndex);

        if (!FNullEnt(pEntity))
        {
            return &this->m_Info[EntityIndex];
        }
    }

    return nullptr;
}

void CPugPlayer::PutInServer(edict_t *pEntity)
{
    auto Player = UTIL_PlayerByIndexSafe(g_engfuncs.pfnIndexOfEdict(pEntity));

    if (Player)
    {
        this->m_Info[Player->entindex()].Clear();
    }
}

void CPugPlayer::ClientDisconnect(edict_t *pEntity)
{
    auto Player = UTIL_PlayerByIndexSafe(g_engfuncs.pfnIndexOfEdict(pEntity));

    if (Player)
    {
        this->m_Info[Player->entindex()].Clear();
    }
}

bool CPugPlayer::HandleMenu_ChooseTeam(CBasePlayer *Player, int Slot)
{
	if (Player->m_iTeam == UNASSIGNED)
	{
		if (!Player->IsBot())
		{
			gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
			gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
		}
	}
    else
    {
        gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Troca de time não permitida.", gPugCvar.m_Tag->string);
        return true;
    }

    return false;
}

void CPugPlayer::GetIntoGame(CBasePlayer* Player)
{
    if (gPugMod.GetState() != STATE_DEAD)
    {
        if (!Player->IsBot())
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 %s Build %s (^3%s^1)", gPugCvar.m_Tag->string, Plugin_info.name, Plugin_info.date, Plugin_info.author);
                gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Digite ^3%shelp^1 para lista de comandos.", gPugCvar.m_Tag->string, gPugCvar.m_CmdPrefixPlayer->string);
            }
        }

        gPugDeathmatch.GetIntoGame(Player);
    }
}

void CPugPlayer::UpdateClientData(CBasePlayer* Player)
{
    gPugDeathmatch.UpdateClientData(Player);
}

bool CPugPlayer::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool TrackChange)
{
    if (gPugDeathmatch.AddAccount(Player, Amount, Type, TrackChange))
    {
        return true;
    }

    return false;
}

bool CPugPlayer::HasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type)
{
    if (gPugMod.GetState() != STATE_DEAD)
    {
        if (gPugCvar.m_RestrictItem)
        {
            if (gPugCvar.m_RestrictItem->string)
            {
                if (item != ITEM_NONE)
                {
                    if (gPugCvar.m_RestrictItem->string[item] != '0')
                    {
                        gPugUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CPugPlayer::GetPlayerSpawnSpot(CBasePlayer* Player)
{
    if (gPugDeathmatch.GetPlayerSpawnSpot(Player))
    {
        return true;
    }

    return false;
}

void CPugPlayer::PlayerSpawn(CBasePlayer* Player)
{
    /**/
}

void CPugPlayer::GiveDefaultItems(CBasePlayer *Player)
{
    gPugDeathmatch.Menu(Player);
}

bool CPugPlayer::FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity)
{
    if (gPugDeathmatch.FPlayerCanTakeDamage(Player, Entity))
    {
        return true;
    }

    return false;
}

void CPugPlayer::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
    gPugDeathmatch.TakeDamage(Player, pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

void CPugPlayer::Killed(CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor)
{   
    gPugDeathmatch.Killed(Victim, pevKiller, pevInflictor);
}

bool CPugPlayer::SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
    if(gPugDeathmatch.SendDeathMessage(Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill))
    {
        return true;
    }

    return false;
}

bool CPugPlayer::ClientCommand(edict_t *pEntity)
{
    if (!FNullEnt(pEntity))
    {
        auto pCommand = g_engfuncs.pfnCmd_Argv(0);

        if (pCommand)
        {
            if (pCommand[0u] != '\0')
            {
                auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

                if (Player)
                {
                    auto EntityIndex = Player->entindex();

                    if (_stricmp(pCommand, "menuselect") == 0)
                    {
                        auto pArg1 = g_engfuncs.pfnCmd_Argv(1);

                        if (pArg1)
                        {
                            if (gPugMenu[EntityIndex].Handle(EntityIndex, std::atoi(pArg1)))
                            {
                                return true;
                            }
                        }
                    }
                    if (_stricmp(pCommand, "drop") == 0)
                    {
                        if(gPugDeathmatch.DropItem(Player))
                        {
                            return true;
                        }
                    }
                    else if (_stricmp(pCommand, "say") == 0 || _stricmp(pCommand, "say_team") == 0)
                    {
                        auto pArg1 = g_engfuncs.pfnCmd_Argv(1);

                        if (pArg1)
                        {
                            if (pArg1[0u] == gPugCvar.m_CmdPrefixPlayer->string[0u] || pArg1[0u] == gPugCvar.m_CmdPrefixAdmin->string[0u])
                            {
                                auto pCmdArgs = g_engfuncs.pfnCmd_Args();

                                if (pCmdArgs)
                                {
                                    if (pCmdArgs[0u] != '\0')
                                    {
                                        g_engfuncs.pfnClientCommand(Player->edict(), strdup("%s\n"), pCmdArgs);
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

void CPugPlayer::SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation)
{
    gPugDeathmatch.SetAnimation(Player, playerAnimation);
}

bool CPugPlayer::ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *szOldMenu)
{
    if (!Player->IsBot())
    {
        return gPugDeathmatch.ShowVGUIMenu(Player, MenuType, BitMask, szOldMenu);
    }

    return false;
}