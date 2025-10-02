#include "precompiled.h"

CPugVoteMap gPugVoteMap;

void CPugVoteMap::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_MapList.clear();
}

void CPugVoteMap::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_MapList.clear();
}

void CPugVoteMap::Init()
{
    this->LoadMaps();
    
    if (this->m_MapList.empty())
    {
        gPugMod.SetState(STATE_VOTETEAM);
    }
    else
    {
        this->m_Run = true;

        this->m_NextFrame = 0.0f;

        this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

        if (g_pGameRules)
        {
            if (gPugCvar.m_MpFreezeTime)
            {
                g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_MpFreezeTime, "120.0");
            }

            CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + 0.1);
            CSGameRules()->m_bCompleteReset = true;
        }

        auto Players = gPugUtil.GetPlayers(true, false);

        this->m_VotesLeft = Players.size();

        for (auto const &Player : Players)
        {
            gPugMenu[Player->entindex()].Create("Escolha o mapa:", false, E_MENU::ME_VOTE_MAP);

            for (size_t i = 0; i < this->m_MapList.size(); ++i)
            {
                this->m_MapList[i].Votes = 0;

                if (!this->m_MapList[i].This && !this->m_MapList[i].Last)
                {
                    gPugMenu[Player->entindex()].AddItem(i, this->m_MapList[i].Name, false, 0);
                }
            }

            gPugUtil.ClientCommand(Player->edict(), g_VoteMap_Sound[g_engfuncs.pfnRandomLong(0, 1)]);

            gPugMenu[Player->entindex()].Show(Player);
        }

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Iniciando a escolha do novo mapa.", gPugCvar.m_Tag->string);
    }
}

void CPugVoteMap::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_NextFrame = 0.0f;

        auto Winner = this->GetWinner();

        gPugUtil.ClientCommand(nullptr, g_VoteMap_Sound[2]);

        if (Winner.Votes)
        {
            gPugTask.Create(E_TASK::VOTE_MAP_CHANGE, 5.0f, false, Winner.Index);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Alterando mapa para: ^3%s^1.", gPugCvar.m_Tag->string, Winner.Name.c_str());
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A escolha falhou: Nenhum voto.", gPugCvar.m_Tag->string);

            if (gPugUtil.GetPlayers(true, true).size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value))
            {
                gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_VOTEMAP);

                gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Reiniciando escolha do mapa.", gPugCvar.m_Tag->string);
            }
            else
            {
                auto Random = this->GetRandom();

                gPugTask.Create(E_TASK::VOTE_MAP_CHANGE, 5.0f, false, Random.Index);

                gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 O Próximo mapa será: ^3%s^1.", gPugCvar.m_Tag->string, Random.Name.c_str());
            }
        }
    }
}

void CPugVoteMap::LoadMaps()
{
    auto MapList = gPugMod.GetMaps();

    if (!MapList.empty())
    {
        this->m_MapList.clear();

        for (const auto& Map : MapList)
        {
            P_VOTE_MAP_INFO MapInfo;

            MapInfo.Index = Map.first;

            MapInfo.Votes = 0;

            MapInfo.Name = Map.second;

            MapInfo.This = (Map.second.compare(STRING(gpGlobals->mapname)) == 0);

            MapInfo.Last = (gPugCvar.m_LastMap->string && (Map.second.compare(gPugCvar.m_LastMap->string) == 0));

            this->m_MapList.push_back(MapInfo);
        }
    }
}

P_VOTE_MAP_INFO CPugVoteMap::GetWinner()
{
    P_VOTE_MAP_INFO Winner = {0, 0, STRING(gpGlobals->mapname), true};

    if (!this->m_MapList.empty())
    {
        for (auto const& Item : this->m_MapList)
        {
            if (Item.Votes >= Winner.Votes)
            {
                Winner = Item;
            }
            else if (Item.Votes == Winner.Votes)
            {
                if (g_engfuncs.pfnRandomLong(0, 1))
                {
                    Winner = Item;
                }
            }
        }
    }

    return Winner;
}

P_VOTE_MAP_INFO CPugVoteMap::GetRandom()
{
    P_VOTE_MAP_INFO Map = {0, 0, STRING(gpGlobals->mapname), true};

    if (!this->m_MapList.empty())
    {
        do
        {
            Map = this->m_MapList.at(g_engfuncs.pfnRandomLong(0, this->m_MapList.size() - 1));
        }
        while (Map.This || Map.Last);
    }

    return Map;
}

void CPugVoteMap::ChangeMap(unsigned int MapIndex)
{
    if (MapIndex < this->m_MapList.size())
    {
        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");

        gPugUtil.ServerCommand("changelevel %s", this->m_MapList[MapIndex].Name.c_str());
    }
}

void CPugVoteMap::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;
            this->m_MapList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 escolheu ^3%s^1.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_MapList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteMap::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            if (!this->m_MapList.empty())
            {
                auto RemainTime = (this->m_Time - time(nullptr));

                if (RemainTime > 0)
                {
                    struct tm *tm_info = localtime(&RemainTime);

                    if (tm_info)
                    {
                        char szTime[32] = { 0 };

                        strftime(szTime, sizeof(szTime), "%M:%S", tm_info);
                        
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[0], "Escolha do Mapa: %s", szTime);
                    }

                    std::string VoteList = "";

                    for (const auto &Item : this->m_MapList)
                    {
                        if (Item.Votes)
                        {
                            VoteList += "[" + std::to_string(Item.Votes) + "] " + Item.Name + "\n";
                        }
                    }

                    if (!VoteList.empty())
                    {
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteMap_HudParam[1], "Nenhum voto registrado.");
                    }
                }
                else
                {
                    this->Stop();
                }
            }

            this->m_NextFrame = (gpGlobals->time + 0.9f);
        }
    }
}
