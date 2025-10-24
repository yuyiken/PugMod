#include "precompiled.h"

CPugDemoRecord gPugDemoRecord;

void CPugDemoRecord::ServerActivate()
{
    this->m_Recording = {};
}

void CPugDemoRecord::ServerDeactivate()
{
    if (gPugCvar.m_DemoRecord->value)
    {
        auto Players = gPugUtil.GetPlayers(true, false);

        for (auto Player : Players)
        {
            if (this->m_Recording[Player->entindex()])
            {
                gPugUtil.ClientCommand(Player->edict(), "stop");
            }
        }

        this->m_Recording = {};
    }
}

void CPugDemoRecord::PlayerSpawn(CBasePlayer *Player)
{
    if (gPugCvar.m_DemoRecord->value)
    {
        auto State = gPugMod.GetState();

        if (State >= STATE_FIRST_HALF && State <= STATE_SECOND_OT)
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                if (!Player->IsBot())
                {
                    if (!this->m_Recording[Player->entindex()])
                    {
                        char File[64] = {0};
                        
                        Q_snprintf(File, sizeof(File), "%ld-%s.dem", time(NULL), STRING(gpGlobals->mapname));

                        gPugUtil.ClientCommand(Player->edict(), "stop; record \"%s\"", File);

                        this->m_Recording[Player->entindex()] = true;

                        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Gravando Demo: ^3%s^1", gPugCvar.m_Tag->string, File);
                    }
                }
            }
        }
        else if (State == STATE_END)
        {
            if (this->m_Recording[Player->entindex()])
            {
                this->m_Recording[Player->entindex()] = false;

                gPugUtil.ClientCommand(Player->edict(), "stop");
            }
        }
    }
}