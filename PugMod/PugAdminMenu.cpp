#include "precompiled.h"

CPugAdminMenu gPugAdminMenu;

void CPugAdminMenu::Menu(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MENU))
    {
        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU, "Menu do Pug Mod:");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_KICK, false, ADMIN_KICK, "Kickar Jogador");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_BAN, false, ADMIN_BAN|ADMIN_BAN_TEMP, "Banir Jogador");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SLAP, false, ADMIN_SLAY, "Tapa no Jogador");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_TEAM, false, ADMIN_LEVEL_A, "Time do Jogador");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_MAP, false, ADMIN_MAP, "Mudar Mapa");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_PUG, false, ADMIN_VOTE|ADMIN_LEVEL_B, "Controlar Pug");

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_CVAR, false, ADMIN_CVAR, "Controlar Cvars");

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (gPugAdmin.CheckAccess(Player, Item.Extra))
        {
            switch (Item.Info)
            {
                case ADMIN_MENU_KICK:
                {
                    this->Kick(Player);
                    break;
                }
                case ADMIN_MENU_BAN:
                {
                    this->Ban(Player);
                    break;
                }
                case ADMIN_MENU_SLAP:
                {
                    this->Slap(Player);
                    break;
                }
                case ADMIN_MENU_TEAM:
                {
                    this->Team(Player);
                    break;
                }
                case ADMIN_MENU_MAP:
                {
                    this->Map(Player);
                    break;
                }
                case ADMIN_MENU_PUG:
                {
                    this->Pug(Player);
                    break;
                }
                case ADMIN_MENU_CVAR:
                {
                    gPugCvarControl.Menu(Player);
                    break;
                }
            }
        }
        else
        {
            this->Menu(Player);
        }
    }
}

void CPugAdminMenu::Kick(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_KICK))
    {
        auto Players = gPugUtil.GetPlayers(false, false);

        if (Players.size() > 0)
        {
            gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_KICK, "Kickar Jogador:");

            for (auto const & Target : Players)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 0, STRING(Target->edict()->v.netname));
                }
            }

            gPugMenu[Player->entindex()].Show(Player);
        }
        else
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
        }
    }
}

void CPugAdminMenu::KickHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            gPugUtil.ClientDrop(Target->entindex(), "Kickado por %s.", STRING(Player->edict()->v.netname));
        }

        this->Kick(Player);
    }
}

void CPugAdminMenu::Ban(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_BAN|ADMIN_BAN_TEMP))
    {
        auto Players = gPugUtil.GetPlayers(false, false);

        if (Players.size() > 0)
        {
            gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_BAN, "Banir Jogador:");

            for (auto const & Target : Players)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, -1, STRING(Target->edict()->v.netname));
                }
            }

            gPugMenu[Player->entindex()].Show(Player);
        }
        else
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
        }
    }
}

void CPugAdminMenu::BanHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            if (Item.Extra < 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_BAN, "Tempo para banir: ^w%s^y", STRING(Target->edict()->v.netname));

                std::vector<time_t> BanTimes = { 5, 10, 15, 30, 60, 120, 240, 480, 960, 1440, 10080, 43200 };

                if (gPugAdmin.Access(Target->entindex(), ADMIN_BAN))
                {
                    BanTimes.insert(BanTimes.begin(), 0);
                }

				for (auto const& Time : BanTimes)
				{
					gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, Time, CTimeFormat::GetTimeLength(Time, CTimeFormat::TIMEUNIT_MINUTES).c_str());
				}

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
				gPugUtil.ServerCommand("banid %d %s", Item.Extra, g_engfuncs.pfnGetPlayerAuthId(Target->edict()));

				gPugUtil.ServerCommand("wait;wait;writeid;writeip");

                gPugUtil.ClientDrop(Target->entindex(), "Você foi banido do servidor.");

                this->Ban(Player);
            }
        }
    }
}

void CPugAdminMenu::Slap(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_SLAY))
    {
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_SLAP, "Tapa no Jogador:");

        for (auto const & Target : Players)
        {
            if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
            {
                if (Target->IsAlive())
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, -1, STRING(Target->edict()->v.netname));
                }
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::SlapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            if (Item.Extra < 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_SLAP, "Tapa: ^w%s^y", STRING(Target->edict()->v.netname));

				gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 0, "Sem Dano");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 1, "Tapa 1 HP");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 5, "Tapa 5 HP");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 10, "Tapa 10 HP");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 25, "Tapa 25 HP");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 50, "Tapa 50 HP");

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), false, 100, "Matar Jogador");

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
                auto Damage = static_cast<float>(Item.Extra);

                if (Target->edict()->v.health > Damage)
                {
                    gPugUtil.ClientSlap(Target->edict(), Damage, false);

                    Item.Extra = -1;

                    this->SlapHandle(Player, Item);
                }
                else
                {
                    gPugUtil.ClientSlap(Target->edict(), Damage, false);

                    this->Slap(Player);
                }
            }
        }
    }
}

void CPugAdminMenu::Team(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_LEVEL_A))
    {
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_TEAM, "Time do Jogador:");

        for (auto Target : Players)
        {
            if (Target->m_iTeam != UNASSIGNED)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(0, false, Target->entindex(), "%s \\R\\y%s", STRING(Target->edict()->v.netname), g_Pug_TeamNameShort[Target->m_iTeam]);
                }
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::TeamHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        auto Target = UTIL_PlayerByIndexSafe(Item.Extra);

        if (Target)
        {
            if (Item.Info == 0)
            {
                gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_TEAM, "Transferir Time: ^w%s^y", STRING(Target->edict()->v.netname));

				gPugMenu[Player->entindex()].AddItem(TERRORIST, Target->m_iTeam == TERRORIST, Target->entindex(), g_Pug_TeamName[TERRORIST]);

                gPugMenu[Player->entindex()].AddItem(CT, Target->m_iTeam == CT, Target->entindex(), g_Pug_TeamName[CT]);

                gPugMenu[Player->entindex()].AddItem(SPECTATOR, Target->m_iTeam == SPECTATOR, Target->entindex(), g_Pug_TeamName[SPECTATOR]);

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
				if (Item.Info == SPECTATOR)
				{
					if (Target->IsAlive())
					{
						Target->edict()->v.deadflag = DEAD_DEAD;

                        Target->edict()->v.health = 0.0f;
					}
				}

                Target->CSPlayer()->JoinTeam((TeamName)Item.Info);

                this->Team(Player);
            }
        }
    }
}

void CPugAdminMenu::Map(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MAP))
    {
        auto MapList = gPugMapList.Get();

        if (MapList.empty())
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum mapa encontrado na lista.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_MAP, "Mudar Mapa:");

        for (const auto& Map : MapList)
        {
            gPugMenu[Player->entindex()].AddItem(Map.first, (Map.second.compare(STRING(gpGlobals->mapname)) == 0), 0, Map.second.c_str());
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::MapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        gPugTask.Create(CHANGE_MAP, 5.0f, false, Item.Info);

        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Alterando mapa para: ^3%s^1.", gPugCvar.m_Tag->string, Item.Text.c_str());
    }
}

void CPugAdminMenu::ChangeMap(int Index)
{
    auto MapList = gPugMapList.Get();

    if (!MapList.empty())
    {
        auto it = MapList.find(Index);

        if (it != MapList.end())
        {
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");

            gPugUtil.ServerCommand("changelevel %s", it->second.c_str());
        }
    }
}

void CPugAdminMenu::Pug(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_VOTE|ADMIN_LEVEL_B))
    {
        gPugMenu[Player->entindex()].Create(true, E_MENU::ME_ADMIN_MENU_PUG, "Escolha uma operação:");

        auto State = gPugMod.GetState();

        if (State >= STATE_DEAD && State <= STATE_KNIFE_ROUND)
        {
            gPugMenu[Player->entindex()].AddItem(STATE_DEAD, false, 0, "Desligar Pug");
            gPugMenu[Player->entindex()].AddItem(STATE_DEATHMATCH, false, 0, "Death Match");
            gPugMenu[Player->entindex()].AddItem(STATE_VOTEMAP, false, 0, "Votação de Mapa");
            gPugMenu[Player->entindex()].AddItem(STATE_VOTETEAM, false, 0, "Votação de Times");
            gPugMenu[Player->entindex()].AddItem(STATE_CAPTAIN, false, 0, "Escolha de Capitães");
            gPugMenu[Player->entindex()].AddItem(STATE_KNIFE_ROUND, false, 0, "Round Faca");
            gPugMenu[Player->entindex()].AddItem(STATE_FIRST_HALF, false, 0, "Iniciar Partida");
        }
        else if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
        {
            gPugMenu[Player->entindex()].AddItem(STATE_FIRST_HALF, false, 0, "Reiniciar Partida");
            gPugMenu[Player->entindex()].AddItem(STATE_DEAD, false, 0, "Cancelar Partida");
            gPugMenu[Player->entindex()].AddItem(STATE_END, false, 0, "Finalizar Partida");
        }

        gPugMenu[Player->entindex()].Show(Player);
    }
}

void CPugAdminMenu::PugHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (Item.Info >= STATE_DEAD && Item.Info <= STATE_END)
        {
            gPugTask.Create(E_TASK::SET_STATE, 0.3f, false, Item.Info);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Estado alterado: ^3%s^1.", gPugCvar.m_Tag->string, g_Pug_String[Item.Info]);
        }
    }
}
