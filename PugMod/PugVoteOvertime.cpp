#include "precompiled.h"

CPugVoteOvertime gPugVoteOvertime;

void CPugVoteOvertime::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteOvertime::CPugVoteOvertime::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Time = 0;

    this->m_VotesLeft = 0;

    this->m_VoteList.clear();
}

void CPugVoteOvertime::Init()
{
    if (!this->m_Run)
    {
        auto Players = gPugUtil.GetPlayers(true, false);

        if (Players.size() > 0)
        {
            this->m_Run = true;

            if (g_pGameRules)
            {
                if (CSGameRules()->m_bRoundTerminating)
                {
                    CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + gPugCvar.m_VoteDelay->value);
                }
            }

            this->m_NextFrame = 0.0f;

            this->m_Time = time(nullptr) + static_cast<time_t>(gPugCvar.m_VoteDelay->value);

            this->m_VotesLeft = 0;

            this->m_VoteList.clear();
            
            this->m_VoteList.push_back({1, 0, "Morte Súbita"});
            this->m_VoteList.push_back({2, 0, "Jogar Overtime"});
            this->m_VoteList.push_back({3, 0, "Terminar Empatado"});
            
            this->m_VotesLeft = Players.size();

            for (auto const &Player : Players)
            {
                gPugMenu[Player->entindex()].Create(false, E_MENU::ME_VOTE_SWAP_TEAM, "Partida empatada o que deseja fazer?");

                for (size_t i = 0; i < this->m_VoteList.size(); ++i)
                {
                    this->m_VoteList[i].Votes = 0;

                    gPugMenu[Player->entindex()].AddItem(i, false, i, this->m_VoteList[i].Name.c_str());
                }

                gPugMenu[Player->entindex()].Show(Player);

                gPugUtil.ClientCommand(Player->edict(), g_VoteOverimte_Sound[g_engfuncs.pfnRandomLong(0, 1)]);
            }

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A partida está empatada, escolha o que fazer:", gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 1. Morte Súbita: ^3O time vencedor será o vencedor da partida.", gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 2. Jogar Overtime: ^3O time vencedor será decidido via Overtime.", gPugCvar.m_Tag->string);
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 3. Terminar Empatado: ^3A partida termina sem um vencedor.", gPugCvar.m_Tag->string);
        }
        else
        {
            gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_END);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Nenhum vencedor: Iniciando ^3%s^1.", gPugCvar.m_Tag->string, g_Pug_String[STATE_END]);
        }
    }
}

void CPugVoteOvertime::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = false;

        this->m_NextFrame = 0.0f;

        this->m_Time = 0;

        this->m_VotesLeft = 0;

        auto Winner = this->GetWinner();

        gPugUtil.ClientCommand(nullptr, g_VoteSwapTeam_Sound[2]);

        if (Winner.Votes)
        {
            switch (Winner.Index)
            {
                case 1: // Sudden Death Round
                {
                    if (g_pGameRules)
                    {
                        CSGameRules()->m_bGameStarted = true;
                    }
                    
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Continuando partida no modo ^3Morte Súbita^1 após o freezetime.", gPugCvar.m_Tag->string);

                    break;
                }
                case 2: // Play Overtime
                {
                    gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_OVERTIME);
                    break;
                }
                case 3: // End Tied
                {
                    gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_END);
                    break;
                }
            }
        }
        else
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A escolha falhou: ^3Nenhum voto.", gPugCvar.m_Tag->string);
            
            gPugTask.Create(E_TASK::SET_STATE, 1.0f, false, STATE_END);
        }
    }
}

P_VOTE_OT_INFO CPugVoteOvertime::GetWinner()
{
    P_VOTE_OT_INFO Winner = {};

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

void CPugVoteOvertime::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (this->m_Run)
    {
        if (Player)
        {
            this->m_VotesLeft -= 1;

            this->m_VoteList[Item.Info].Votes += 1;

            gPugUtil.PrintColor(nullptr, (Item.Info == TERRORIST) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE, "^4[%s]^1 ^3%s^1 escolheu ^3%s^1.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname), this->m_VoteList[Item.Info].Name.c_str());

            if (this->m_VotesLeft < 1)
            {
                this->Stop();
            }
        }
    }
}

void CPugVoteOvertime::StartFrame()
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
                        
                        gPugUtil.SendHud(nullptr, g_VoteSwapTeam_HudParam[0], "Modo Overtime: %s", szTime);
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
