#include "precompiled.h"

CPugAdminMenu gPugAdminMenu;

void CPugAdminMenu::Menu(CBasePlayer *Player)
{
    auto Flags = gPugAdmin.GetFlags(Player->entindex());

    if (!(Flags & ADMIN_MENU))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

    gPugMenu[Player->entindex()].Create("Menu do Pug Mod:", true, E_MENU::ME_ADMIN_MENU);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_KICK, "Kickar Jogador", !(Flags & ADMIN_KICK), 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_BAN, "Banir Jogador", !(Flags & ADMIN_BAN) && !(Flags & ADMIN_BAN_TEMP), 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SLAP, "Tapa no Jogador", !(Flags & ADMIN_SLAY), 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_TEAM, "Time do Jogador", !(Flags & ADMIN_LEVEL_A), 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_MAP, "Mudar Mapa", !(Flags & ADMIN_MAP), 0);

    gPugMenu[Player->entindex()].Show(Player);
}

void CPugAdminMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (Item.Disabled)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
            return;
        }

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
        }
    }
}

void CPugAdminMenu::Kick(CBasePlayer *Player)
{
    if (!gPugAdmin.Access(Player->entindex(), ADMIN_KICK))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

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
    auto Flags = gPugAdmin.GetFlags(Player->entindex());

    if (!(Flags & ADMIN_BAN) && !(Flags & ADMIN_BAN_TEMP))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

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
    if (!gPugAdmin.Access(Player->entindex(), ADMIN_SLAY))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

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
    if (!gPugAdmin.Access(Player->entindex(), ADMIN_LEVEL_A))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

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
        //if (!gPugAdmin.Access(Target->entindex(), ADMIN_IMMUNITY))
        {
            if (Target->m_iTeam != UNASSIGNED)
            {
                gPugMenu[Player->entindex()].AddItem(Target->entindex(), gPugUtil.FormatString("%s \\R\\y%s", STRING(Target->edict()->v.netname), g_Pug_TeamShort[Player->m_iTeam]), false, -1);
            }
        }
    }

    gPugMenu[Player->entindex()].Show(Player);
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

}

void CPugAdminMenu::MapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}
