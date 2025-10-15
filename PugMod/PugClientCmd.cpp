#include "precompiled.h"

CPugClientCmd gPugClientCmd;

void CPugClientCmd::ServerActivate()
{
    this->m_Data.clear();

    gPugUtil.ServerCommand("exec %s/cfg/clientcmd.cfg", gPugUtil.GetPath());
}

void CPugClientCmd::Set(std::string Index, std::string Name, std::string Flags)
{
    if (!Index.empty() && !Name.empty() && !Flags.empty())
    {
        if (this->m_Data.find(Name) == this->m_Data.end())
        {
            this->m_Data[Name].Index = std::stoi(Index);

            this->m_Data[Name].Name = Name;

            this->m_Data[Name].Flags = gPugAdmin.ReadFlags(Flags.c_str());
        }
    }
}

LP_CLIENT_CMD CPugClientCmd::Get(const char *Name)
{
    if (Name)
    {
        if (this->m_Data.find(Name) != this->m_Data.end())
        {
            return &this->m_Data.at(Name);
        }
    }

    return nullptr;
}

LP_CLIENT_CMD CPugClientCmd::Get(int Index)
{
    for (auto const &Command : this->m_Data)
    {
        if (Command.second.Index == Index)
        {
            return &this->m_Data.at(Command.first);
        }
    }

    return nullptr;
}

bool CPugClientCmd::FilterFlood(const char *pszCommand, int EntityIndex)
{
    if (!gPugAdmin.Access(EntityIndex, ADMIN_IMMUNITY))
    {
        if (_stricmp(pszCommand, "say") == 0 || _stricmp(pszCommand, "say_team") == 0 || _stricmp(pszCommand, "jointeam") == 0 || _stricmp(pszCommand, "chooseteam") == 0)
        {
            if (this->m_FloodTimer[EntityIndex] > gpGlobals->time)
            {
                if (this->m_FloodCount[EntityIndex] >= CMD_MAX_FLOOD_REPEAT)
                {
                    this->m_FloodTimer[EntityIndex] = (gpGlobals->time + CMD_MIN_FLOOD_TIME + CMD_MIN_FLOOD_NEXT_TIME);
                    return true;
                }

                this->m_FloodCount[EntityIndex]++;
            }
            else if (this->m_FloodCount[EntityIndex])
            {
                this->m_FloodCount[EntityIndex]--;
            }

            this->m_FloodTimer[EntityIndex] = gpGlobals->time + CMD_MIN_FLOOD_TIME;
        }
    }

    return false;
}

bool CPugClientCmd::Command(edict_t *pEntity)
{
    if (!FNullEnt(pEntity))
    {
        const char *pszCommand = g_engfuncs.pfnCmd_Argv(0);

        if (pszCommand)
        {
            if (pszCommand[0u] != '\0')
            {
                auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

                if (Player)
                {              
                    if (this->FilterFlood(pszCommand, Player->entindex()))
                    {
                        gPugUtil.ClientPrint(pEntity, E_PRINT::CONSOLE, "*** FLOODING DETECTADO ***");
                        return true;
                    }

                    if (_stricmp(pszCommand, "menuselect") == 0)
                    {
                        auto pszArgv = g_engfuncs.pfnCmd_Argv(1);

                        if (pszArgv)
                        {
                            if (gPugMenu[Player->entindex()].Handle(Player, std::atoi(pszArgv)))
                            {
                                return true;
                            }
                        }
                    }
                    else if (_stricmp(pszCommand, "drop") == 0)
                    {
                        if(gPugDM.DropItem(Player))
                        {
                            return true;
                        }
                    }
                    else if (_stricmp(pszCommand, "say") == 0 || _stricmp(pszCommand, "say_team") == 0)
                    {
                        auto pszArgv = g_engfuncs.pfnCmd_Argv(1);

                        if (pszArgv)
                        {
                            if (pszArgv[0u] == gPugCvar.m_CmdPrefixPlayer->string[0u] || pszArgv[0u] == gPugCvar.m_CmdPrefixAdmin->string[0u])
                            {
                                auto pszCmdArgs = g_engfuncs.pfnCmd_Args();

                                if (pszCmdArgs)
                                {
                                    if (pszCmdArgs[0u] != '\0')
                                    {
                                        auto Command = strdup("%s\n");

                                        g_engfuncs.pfnClientCommand(pEntity, Command, pszCmdArgs);

                                        free(Command);

                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        auto pCmd = this->Get(pszCommand);

                        if (pCmd)
                        {
                            if (gPugAdmin.Access(Player->entindex(), pCmd->Flags))
                            {
                                switch(pCmd->Index)
                                {
                                    case CMD_STATUS:
                                    {
                                        gPugMod.Status(Player);
                                        return true;
                                    }
                                    case CMD_SCORE:
                                    {
                                        gPugMod.Scores(Player);
                                        return true;
                                    }
                                    case CMD_READY:
                                    {
                                        gPugReady.Ready(Player);
                                        return true;
                                    }
                                    case CMD_NOTREADY:
                                    {
                                        gPugReady.NotReady(Player);
                                        return true;
                                    }
                                    case CMD_MENU:
                                    {
                                        return true;
                                    }
                                    case CMD_GUNS:
                                    {
                                        gPugDM.DropItem(Player);
                                        return true;
                                    }
                                    case CMD_RESPAWN:
                                    {
                                        gPugDM.Respawn(Player);
                                        return true;
                                    }
                                    case CMD_RESET_SCORE:
                                    {
                                        gPugDM.ResetScore(Player);
                                        return true;
                                    }
                                    case CMD_DM_SPAWN_EDITOR:
                                    {
                                        gPugSpawnEdit.EditSpawns(Player);
                                        return true;
                                    }
                                    case CMD_HELP:
                                    {
                                        this->Help(Player);
                                        return true;
                                    }
                                    case CMD_HELP_ADMIN:
                                    {
                                        this->HelpAdmin(Player);
                                        return true;
                                    }
                                    case CMD_ADMIN_MENU:
                                    {
                                        gPugAdminMenu.Menu(Player);
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

bool CPugClientCmd::Help(CBasePlayer *Player)
{
    if (gPugCvar.m_MotdFile)
    {
        if (gPugCvar.m_MotdFile->string)
        {
            if (gPugCvar.m_MotdFile->string[0u] != '\0')
            {
                gPugUtil.ShowMotd(Player->edict(), gPugCvar.m_MotdFile->string, strlen(gPugCvar.m_MotdFile->string));
                
                return true;
            }
        }
    }

    return false;
}

bool CPugClientCmd::HelpAdmin(CBasePlayer *Player)
{
    if (gPugAdmin.Access(Player->entindex(), ADMIN_LEVEL_C))
    {
        if (gPugCvar.m_MotdFileAdmin)
        {
            if (gPugCvar.m_MotdFileAdmin->string)
            {
                if (gPugCvar.m_MotdFileAdmin->string[0u] != '\0')
                {
                    gPugUtil.ShowMotd(Player->edict(), gPugCvar.m_MotdFileAdmin->string, strlen(gPugCvar.m_MotdFileAdmin->string));
                    
                    return true;
                }
            }
        }
    }

    return false;
}