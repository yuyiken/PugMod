#include "precompiled.h"

CPugAdminMenu gPugAdminMenu;

void CPugAdminMenu::Menu(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MENU))
    {
        gPugMenu[Player->entindex()].Create("Menu do Pug Mod:", true, E_MENU::ME_ADMIN_MENU);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_KICK, "Kickar Jogador", false, ADMIN_KICK);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_BAN, "Banir Jogador", false, ADMIN_BAN|ADMIN_BAN_TEMP);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SLAP, "Tapa no Jogador", false, ADMIN_SLAY);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_TEAM, "Time do Jogador", false, ADMIN_LEVEL_A);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_MAP, "Mudar Mapa", false, ADMIN_MAP);

        gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_PUG, "Controlar Pug", false, ADMIN_VOTE|ADMIN_LEVEL_B);

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
            }
        }
    }
}

void CPugAdminMenu::Kick(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_KICK))
    {
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create("Kickar Jogador:", true, E_MENU::ME_ADMIN_MENU_KICK);

        for (auto const & Target : Players)
        {
            if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
            {
                gPugMenu[Player->entindex()].AddItem(Target->entindex(), STRING(Target->edict()->v.netname), false, 0);
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
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
        auto Players = gPugUtil.GetPlayers(false, true);

        if (Players.size() < 1)
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum jogador encontrado.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create("Banir Jogador:", true, E_MENU::ME_ADMIN_MENU_BAN);

        for (auto const & Target : Players)
        {
            if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
            {
                gPugMenu[Player->entindex()].AddItem(Target->entindex(), STRING(Target->edict()->v.netname), false, -1);
            }
        }

        gPugMenu[Player->entindex()].Show(Player);
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
                gPugMenu[Player->entindex()].Create(gPugUtil.FormatString("Tempo para banir: ^w%s^y", STRING(Target->edict()->v.netname)), true, E_MENU::ME_ADMIN_MENU_BAN);

                std::vector<time_t> BanTimes = { 5, 10, 15, 30, 60, 120, 240, 480, 960, 1440, 10080, 43200 };

                if (gPugAdmin.Access(Target->entindex(), ADMIN_BAN))
                {
                    BanTimes.insert(BanTimes.begin(), 0);
                }

				for (auto const& Time : BanTimes)
				{
					gPugMenu[Player->entindex()].AddItem(Target->entindex(), CTimeFormat::GetTimeLength(Time, CTimeFormat::TIMEUNIT_MINUTES), false, Time);
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

        gPugMenu[Player->entindex()].Create("Tapa no Jogador:", true, E_MENU::ME_ADMIN_MENU_SLAP);

        for (auto const & Target : Players)
        {
            if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
            {
                if (Target->IsAlive())
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), STRING(Target->edict()->v.netname), false, -1);
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
                gPugMenu[Player->entindex()].Create(gPugUtil.FormatString("Tapa: ^w%s^y", STRING(Target->edict()->v.netname)), true, E_MENU::ME_ADMIN_MENU_SLAP);

				gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Sem Dano", false, 0);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Tapa 1 HP", false, 1);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Tapa 5 HP", false, 5);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Tapa 10 HP", false, 10);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Tapa 25 HP", false, 25);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Tapa 50 HP", false, 50);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), "Matar Jogador", false, 100);

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

        gPugMenu[Player->entindex()].Create("Time do Jogador:", true, E_MENU::ME_ADMIN_MENU_TEAM);

        for (auto const & Target : Players)
        {
            if (Target->m_iTeam != UNASSIGNED)
            {
                if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
                {
                    gPugMenu[Player->entindex()].AddItem(Target->entindex(), gPugUtil.FormatString("%s \\R\\y%s", STRING(Target->edict()->v.netname), g_Pug_TeamShort[Player->m_iTeam]), false, -1);
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
        auto Target = UTIL_PlayerByIndexSafe(Item.Info);

        if (Target)
        {
            if (Item.Extra < 0)
            {
                gPugMenu[Player->entindex()].Create(gPugUtil.FormatString("Transferir Time: ^w%s^y", STRING(Target->edict()->v.netname)), true, E_MENU::ME_ADMIN_MENU_TEAM);

				gPugMenu[Player->entindex()].AddItem(Target->entindex(), g_Pug_TeamId[TERRORIST], Target->m_iTeam == TERRORIST, TERRORIST);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), g_Pug_TeamId[CT], Target->m_iTeam == CT, CT);

                gPugMenu[Player->entindex()].AddItem(Target->entindex(), g_Pug_TeamId[SPECTATOR], Target->m_iTeam == SPECTATOR, SPECTATOR);

                gPugMenu[Player->entindex()].Show(Player);
            }
            else
            {
				if (Item.Extra == SPECTATOR)
				{
					if (Target->IsAlive())
					{
						Target->edict()->v.deadflag = DEAD_DEAD;

                        Target->edict()->v.health = 0.0f;
					}
				}

                Target->CSPlayer()->JoinTeam((TeamName)Item.Extra);

                this->Team(Player);
            }
        }
    }
}

void CPugAdminMenu::Map(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_MAP))
    {
        auto MapList = gPugMod.GetMaps();

        if (MapList.empty())
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum mapa encontrado na lista.", gPugCvar.m_Tag->string);
            return;
        }

        gPugMenu[Player->entindex()].Create("Mudar Mapa:", true, E_MENU::ME_ADMIN_MENU_MAP);

        for (const auto& Map : MapList)
        {
            gPugMenu[Player->entindex()].AddItem(Map.first, Map.second, (Map.second.compare(STRING(gpGlobals->mapname)) == 0), 0);
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
    auto MapList = gPugMod.GetMaps();

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
        auto State = gPugMod.GetState();

        if (State > STATE_DEAD && State < STATE_END)
        {
            gPugMenu[Player->entindex()].Create("Escolha uma ação a executar:", true, E_MENU::ME_ADMIN_MENU_PUG);

            if (State == STATE_DEATHMATCH)
            {
                gPugMenu[Player->entindex()].AddItem(0, g_Pug_String[STATE_VOTEMAP], false, STATE_VOTEMAP);
                gPugMenu[Player->entindex()].AddItem(1, g_Pug_String[STATE_VOTETEAM], false, STATE_VOTETEAM);
                gPugMenu[Player->entindex()].AddItem(2, g_Pug_String[STATE_CAPTAIN], false, STATE_CAPTAIN);
                gPugMenu[Player->entindex()].AddItem(3, g_Pug_String[STATE_KNIFE_ROUND], false, STATE_KNIFE_ROUND);
                gPugMenu[Player->entindex()].AddItem(4, g_Pug_String[STATE_FIRST_HALF], false, STATE_FIRST_HALF);
            }
            else if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
            {
                gPugMenu[Player->entindex()].AddItem(5, "Reiniciar Partida", false, STATE_FIRST_HALF);
                gPugMenu[Player->entindex()].AddItem(6, "Finalizar Partida", false, STATE_END);
                gPugMenu[Player->entindex()].AddItem(7, "Cancelar Partida", false, STATE_DEATHMATCH);
                gPugMenu[Player->entindex()].AddItemFormat(8, false, State, "Reiniciar %s", g_Pug_String[State]);
            }

            gPugMenu[Player->entindex()].Show(Player);
        }
        else
        {
            this->Menu(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Controle indisponível nesse estado.", gPugCvar.m_Tag->string);
        }
    }
}

void CPugAdminMenu::PugHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (Item.Extra == STATE_FIRST_HALF)
        {
            gPugReady.Stop(true);
            gPugTimer.Stop(true);
            
            gPugMod.SetScores(STATE_FIRST_HALF, 2, 1);
            gPugMod.SetScores(STATE_SECOND_HALF, 1, 2);
            gPugMod.SetScores(STATE_OVERTIME, 0, 0);

            if (g_pGameRules)
            {
                CSGameRules()->m_iNumCTWins = 3;
                CSGameRules()->m_iNumTerroristWins = 3;
            }

            gPugMod.SetState(STATE_OVERTIME);
        }

        //gPugMod.SetScores(Item.Extra, 0, 0);

        //gPugMod.SetState(Item.Extra);
    }
}
