#include "precompiled.h"

CPugDemoRecord gPugDemoRecord;

void CPugDemoRecord::ServerActivate()
{
    this->m_Asked = {};
}

void CPugDemoRecord::ServerDeactivate()
{
    this->m_Asked = {};

    if (gPugCvar.m_DemoRecord->value)
    {
        auto Players = gPugUtil.GetPlayers(false, false);

        for (auto Player : Players)
        {
            gPugUtil.ClientCommand(Player->edict(), "stop");
        }
    }
}

void CPugDemoRecord::GetIntoGame(CBasePlayer *Player)
{
    if (gPugCvar.m_DemoRecord->value)
    {
        this->m_Asked[Player->entindex()] = false;
    }
}

void CPugDemoRecord::PlayerSpawn(CBasePlayer *Player)
{
    if (gPugCvar.m_DemoRecord->value)
    {
        auto State = gPugMod.GetState();

        if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
        {
            if (!this->m_Asked[Player->entindex()])
            {
                this->Menu(Player);
            }
        }
    }
}

void CPugDemoRecord::Menu(CBasePlayer *Player)
{
    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT || Player->m_iTeam == SPECTATOR)
    {
        if (!Player->IsBot())
        {
            gPugMenu[Player->entindex()].Create(false, E_MENU::ME_DEMO_RECORD, "Gravar demo?");

            gPugMenu[Player->entindex()].AddItem(1, false, 1, "Sim");

            gPugMenu[Player->entindex()].AddItem(0, false, 0, "Não");

            gPugMenu[Player->entindex()].Show(Player);

            this->m_Asked[Player->entindex()] = true;
        }
    }
}

void CPugDemoRecord::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        if (Item.Info)
        {
            char File[128] = {0};
            
            Q_snprintf(File, sizeof(File), "pug-%s-%u.dem", STRING(gpGlobals->mapname), (unsigned int)time(NULL));

            gPugUtil.ClientCommand(Player->edict(), "stop; record \"pug-%s-%u.dem\"", File);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Gravando: ^3%s^1", gPugCvar.m_Tag->string, File);
        }
        else
        {
            gPugUtil.ClientCommand(Player->edict(), "stop");
        }
    }
}