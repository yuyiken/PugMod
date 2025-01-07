#include "precompiled.h"

CPugVoteSwapTeam gPugVoteSwapTeam;

void CPugVoteSwapTeam::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_Team = UNASSIGNED;

    this->m_VoteList.push_back({TERRORIST, 0, "Terroristas"});
    this->m_VoteList.push_back({CT, 0, "Contra-Terroristas"});
}

void CPugVoteSwapTeam::CPugVoteSwapTeam::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_Team = UNASSIGNED;

    this->m_VoteList.clear();
}

void CPugVoteSwapTeam::Init(TeamName Team)
{
    this->m_Run = true;

    this->m_NextFrame = 0.0f;

    this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

    this->m_Team = Team;

    auto Players = gPugUtil.GetPlayers(this->m_Team);

    if (Players.size())
    {
        for (auto const &Player : Players)
        {
            gPugMenu[Player->entindex()].Create("Escolha o Time:", false, E_MENU::ME_VOTE_SWAP_TEAM);

            for (auto i = 0; i < this->m_VoteList.size(); ++i)
            {
                this->m_VoteList[i].Votes = 0;

                gPugMenu[Player->entindex()].AddItem(this->m_VoteList[i].Team, this->m_VoteList[i].Name, false, Player->entindex());
            }

            gPugMenu[Player->entindex()].Show(Player);

            gPugUtil.ClientCommand(Player->edict(), g_VoteMap_Sound[g_engfuncs.pfnRandomLong(0, 1)]);
        }

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Iniciando a escolha do time.", gPugCvar.m_Tag->string);
    }
    else
    {
        // Do not swap and go to next state
    }
}

void CPugVoteSwapTeam::Stop()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    auto Winner = this->GetWinner();

    gPugUtil.ClientCommand(nullptr, g_VoteSwapTeam_Sound[2]);

    if (Winner.Votes)
    {
        if (Winner.Team == this->m_Team)
        {
            if (g_pGameRules)
            {
                CSGameRules()->SwapAllPlayers();
            }
        }

        gPugUtil.PrintColor(nullptr, (this->m_Team == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), "^4[%s]^1 Trocando times para: ^3%s^1.", gPugCvar.m_Tag->string, Winner.Name.c_str());
    }
    else
    {
        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A escolha falhou: ^3Nenhum voto.", gPugCvar.m_Tag->string);
    }

    gPugMod.SetState(STATE_FIRST_HALF);
}

P_VOTE_SWAP_INFO CPugVoteSwapTeam::GetWinner()
{
    P_VOTE_SWAP_INFO Winner = {};

    if (!this->m_VoteList.empty())
    {
        for (auto const& Item : this->m_VoteList)
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

void CPugVoteSwapTeam::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            if (Item.Info < this->m_VoteList.size())
            {
                this->m_VoteList[Item.Info].Votes += 1;
                
                gPugUtil.PrintColor
                (
                    nullptr,
                    (Item.Info == TERRORIST) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE,
                    "^4[%s]^1 ^3%s^1 escolheu ^3%s^1.",
                    gPugCvar.m_Tag->string,
                    STRING(Player->edict()->v.netname),
                    this->m_VoteList[Item.Info].Name.c_str()
                );
            }
        }
    }
}

void CPugVoteSwapTeam::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            if (!this->m_VoteList.empty())
            {
                auto RemainTime = (this->m_Time - time(nullptr));

                if (RemainTime > 0)
                {
                    struct tm *tm_info = localtime(&RemainTime);

                    if (tm_info)
                    {
                        char szTime[32] = { 0 };

                        strftime(szTime, sizeof(szTime), "%M:%S", tm_info);
                        
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[0], "Escolha do Time: %s", szTime);
                    }

                    std::string VoteList = "";

                    for (const auto &Item : this->m_VoteList)
                    {
                        if (Item.Votes)
                        {
                            VoteList += "[" + std::to_string(Item.Votes) + "] " + Item.Name + "\n";
                        }
                    }

                    if (!VoteList.empty())
                    {
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[1], VoteList.c_str());
                    }
                    else
                    {
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[1], "Nenhum voto registrado.");
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
