#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    this->SetState(STATE_DEAD);
}

void CPugMod::ServerDeactivate()
{
    if (gPugCvar.m_LastMap)
    {
        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_LastMap, STRING(gpGlobals->mapname));
    }
    
	if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
	{
		this->SetState(STATE_END);
	}
}

int CPugMod::GetState()
{
    return this->m_State;
}

int CPugMod::SetState(int State)
{
    this->m_State = State;

    this->m_NextFrame = 0.0f;

    switch(this->m_State)
    {
        case STATE_DEAD:
        {
            gPugTask.Create(E_TASK::SET_STATE, 5.0f, false, STATE_DEATHMATCH);
            break;
        }
        case STATE_DEATHMATCH:
        {
            if (gPugCvar.m_ReadyType->value == 1.0f)
            {
                gPugTimer.Init();
            }
            else if (gPugCvar.m_ReadyType->value == 2.0f)
            {
                gPugReady.Init();
            }

            if (gPugCvar.m_DM_Enable->value)
            {
                gPugDM.Init();
            }
            
            break;
        }
        case STATE_VOTEMAP:
        {
            gPugDM.Stop();

            gPugVoteMap.Init();
            break;
        }
        case STATE_VOTETEAM:
        {
            gPugDM.Stop();

            gPugVoteTeam.Init();
            break;
        }
        case STATE_CAPTAIN:
        {
            gPugLeader.Init();
            break;
        }
        case STATE_KNIFE_ROUND:
        {
            this->m_Score[this->m_State].fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Round Faca: O ^3vencedor escolhe o time inicial.", gPugCvar.m_Tag->string);

            gPugLO3.Init();
            break;
        }
        case STATE_FIRST_HALF:
        {
            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }
            
            this->m_Score[this->m_State].fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se!!.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);

            gPugLO3.Init();
            break;
        }
        case STATE_HALFTIME:
        {
            this->SwapTeams();

            auto Players = gPugUtil.GetPlayers(true, true);

            if (Players.size() < static_cast<int>(gPugCvar.m_PlayersMin->value))
            {
                if (gPugCvar.m_ReadyType->value == 1.0f)
                {
                    gPugTimer.Init();
                }
                else if (gPugCvar.m_ReadyType->value == 2.0f)
                {
                    gPugReady.Init();
                }
            }
            else
            {
                if (this->GetRound() < static_cast<int>(gPugCvar.m_Rounds->value))
                {
                    gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_SECOND_HALF);
                }
                else
                {
                    gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_OVERTIME);
                }
            }

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se!!", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
            break;
        }
        case STATE_SECOND_HALF:
        {
            this->m_Score[this->m_State].fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se!!.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);

            gPugLO3.Init();
            break;
        }
        case STATE_OVERTIME:
        {
            this->m_Score[this->m_State].fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se!!.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);

            gPugLO3.Init();
            break;
        }
        case STATE_END:
        {
            break;
        }
    }

    gPugUtil.ServerCommand("exec %s/cfg/%s.cfg", gPugUtil.GetPath(), g_Pug_Config[this->m_State]);

    return this->m_State;
}

void CPugMod::NextState(float Delay)
{
    if (this->m_State == STATE_DEATHMATCH)
    {
        if (gPugCvar.m_VoteMap->value != 0.0f)
        {
            gPugTask.Create(E_TASK::SET_STATE, Delay, false, STATE_VOTEMAP);

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");
        }
        else
        {
            gPugTask.Create(E_TASK::SET_STATE, Delay, false, STATE_VOTETEAM);

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");
        }
    }
    else
    {
        if (this->GetRound() < static_cast<int>(gPugCvar.m_Rounds->value))
        {
            gPugTask.Create(E_TASK::SET_STATE, Delay, false, STATE_SECOND_HALF);
        }
        else
        {
            gPugTask.Create(E_TASK::SET_STATE, Delay, false, STATE_OVERTIME);
        }
    }
}

void CPugMod::SwapTeams()
{
    if (this->GetRound() != static_cast<int>(gPugCvar.m_Rounds->value))
    {
        if (g_pGameRules)
        {
            CSGameRules()->SwapAllPlayers();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_SvRestart, "1");
        }

        for (int State = STATE_DEAD; State <= STATE_END; State++)
        {
            SWAP(this->m_Score[State][TERRORIST], this->m_Score[State][CT]);
        }

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Trocando times automaticamente.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
    }
}

std::array<int, SPECTATOR> CPugMod::GetScore()
{
    std::array<int, SPECTATOR> Result = {};

    for (int State = STATE_DEAD; State <= STATE_END; State++)
    {
        if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
        {
            for (int Team = TERRORIST; Team <= CT; Team++)
            {
                Result[Team] += this->m_Score[State][Team];
            }
        }
    }

    return Result;
}

int CPugMod::GetRound()
{
    auto Score = this->GetScore();

    return (Score[TERRORIST] + Score[CT]);
}

int CPugMod::GetWinner()
{
    auto Scores = this->GetScore();

    if (Scores[TERRORIST] != Scores[CT])
    {
        if (Scores[TERRORIST] > Scores[CT])
        {
            return TERRORIST;
        }
        else if (Scores[TERRORIST] < Scores[CT])
        {
            return CT;
        }
    }

    return UNASSIGNED;
}

bool CPugMod::ChooseTeam(CBasePlayer *Player, int Slot)
{
    if (Player->m_iTeam == UNASSIGNED)
    {
        if (!Player->IsBot())
        {
            gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + TERRORIST + 1, "TERRORIST");
            gPugUtil.TeamInfo(Player->edict(), MAX_CLIENTS + CT + 1, "CT");
        }
    }

    if (Slot == MENU_SLOT_TEAM_RANDOM)
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Seleção automática não permitida.", gPugCvar.m_Tag->string);
        return true;
    }

    if (Slot == MENU_SLOT_TEAM_SPECT)
    {
        if (gPugCvar.m_AllowSpectators->value < 1.0f)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Espectadores não permitidos.", gPugCvar.m_Tag->string);
            return true;
        }
    }

    if (Player->m_iTeam == Slot)
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você já está nesse time.", gPugCvar.m_Tag->string);
        return true;
    }

    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
    {
        if (this->m_State >= STATE_VOTEMAP && this->m_State <= STATE_OVERTIME)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Troca de time não permitida durante a partida.", gPugCvar.m_Tag->string);
            return true;
        }
    }

	if (Slot == MENU_SLOT_TEAM_TERRORIST || Slot == MENU_SLOT_TEAM_CT)
	{
        auto Players = gPugUtil.GetPlayers();

		if (Players[Slot].size() >= static_cast<size_t>(gPugCvar.m_PlayersMax->value / 2.0f))
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Esse time já está completo.", gPugCvar.m_Tag->string);
			return true;
		}
	}

    return false;
}

void CPugMod::GetIntoGame(CBasePlayer *Player)
{
    if (this->m_State != STATE_DEAD)
    {
        if (!Player->IsBot())
        {
            if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
            {
                if (!Player->m_iLastAccount)
                {
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 %s Build %s (^3%s^1)", gPugCvar.m_Tag->string, Plugin_info.name, Plugin_info.date, Plugin_info.author);
                    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Digite ^3%shelp^1 para lista de comandos.", gPugCvar.m_Tag->string, gPugCvar.m_CmdPrefixPlayer->string);
                }
            }
        }
    }
}

bool CPugMod::HasRestrictItem(CBasePlayer* Player, ItemID Item, ItemRestType Type)
{
    if (this->m_State != STATE_DEAD)
    {
        if (gPugCvar.m_RestrictItem)
        {
            if (gPugCvar.m_RestrictItem->string)
            {
                if (Item >= ITEM_SHIELDGUN && Item <= ITEM_BATTERY)
                {
                    if (gPugCvar.m_RestrictItem->string[Item] != '0')
                    {
                        gPugUtil.ClientPrint(Player->edict(), E_PRINT::CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
                        return true;
                    }

                    if (this->m_State == STATE_KNIFE_ROUND)
                    {
                        if ((Item != ITEM_KEVLAR) && (Item != ITEM_ASSAULT) && (Item != ITEM_KNIFE))
                        {
                            gPugUtil.ClientPrint(Player->edict(), E_PRINT::CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void CPugMod::DropClient(edict_t *pEntity)
{
    if (this->m_State >= STATE_KNIFE_ROUND && this->m_State <= STATE_OVERTIME)
    {
        auto Players = gPugUtil.GetPlayers();

        auto PlayersMin = static_cast<int>((gPugCvar.m_PlayersMin->value / 2.0f) - 1.0f);

        if((Players[TERRORIST].size() < PlayersMin) || (Players[CT].size() < PlayersMin))
        {
            if (Players[SPECTATOR].size() < 2)
            {
                gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_END);
            }
        }
    }
}

void CPugMod::HudMsg()
{
    if (this->m_State >= STATE_KNIFE_ROUND && this->m_State <= STATE_END)
    {
        auto Score = this->GetScore();

        auto Round = (Score[TERRORIST] + Score[CT] + 1);

        auto Total = static_cast<int>(gPugCvar.m_Rounds->value);

        auto Quite = (Total / 2);

        if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF)
        {
            if (Round == 1)
            {
                gPugUtil.SendHud(nullptr, g_Pug_HudParam, "Iniciando:\n%s", g_Pug_String[this->m_State]);
            }
            else
            {
                if (Round < Quite)
                {
                    gPugUtil.SendHud(nullptr, g_Pug_HudParam, "%s\nRound %d\nTRs %d - %d CTs", g_Pug_String[this->m_State], Round, Score[TERRORIST], Score[CT]);
                }
                else if (Round == Quite)
                {
                    gPugUtil.SendHud(nullptr, g_Pug_HudParam, "Último Round\n%s\nTRs %d - %d CTs", g_Pug_String[this->m_State], Score[TERRORIST], Score[CT]);
                }
            }
        }
        if (this->m_State == STATE_SECOND_HALF)
        {
            if (Score[TERRORIST] < Quite && Score[CT] < Quite)
            {
                gPugUtil.SendHud(nullptr, g_Pug_HudParam, "%s\nRound %d\nTRs %d - %d CTs", g_Pug_String[this->m_State], Round, Score[TERRORIST], Score[CT]);
            }
            else
            {
                gPugUtil.SendHud(nullptr, g_Pug_HudParam, "Último Round GG\n%s\nTRs %d - %d CTs", g_Pug_String[this->m_State], Score[TERRORIST], Score[CT]);
            }
        }
    }
}

void CPugMod::RestartRound()
{
    if (g_pGameRules)
    {
        if (CSGameRules()->m_bCompleteReset)
        {
            this->m_Score[this->m_State].fill(0);
        }
        else
        {
            gPugTask.Create(E_TASK::SEND_HUDMESSAGE, 0.1f, false, 0);
        }
    }
}

void CPugMod::RoundStart()
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        this->Scores(nullptr);

        gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Iniciado.", gPugCvar.m_Tag->string, this->GetRound());
    }
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (winStatus != WINSTATUS_NONE)
        {
            auto Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

            if (g_pGameRules)
            {
                this->m_Score[this->m_State][TERRORIST] = CSGameRules()->m_iNumTerroristWins;
                this->m_Score[this->m_State][CT]        = CSGameRules()->m_iNumCTWins;
            }

            gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Ganho Por: %s.", gPugCvar.m_Tag->string, this->GetRound(), g_Pug_TeamId[Winner]);

            switch (this->m_State)
            {
                case STATE_KNIFE_ROUND:
                {
                    gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, Winner);
                    break;
                }
                case STATE_FIRST_HALF:
                {
                    auto Score = this->GetScore();

                    if ((Score[TERRORIST] + Score[CT]) >= static_cast<int>(gPugCvar.m_Rounds->value / 2.0f))
                    {
                        gPugTask.Create(E_TASK::SET_STATE, tmDelay + 1.0f, false, STATE_HALFTIME);
                    }
                    break;
                }
                case STATE_SECOND_HALF:
                {
                    auto Score = this->GetScore();

                    auto Quite = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                    if ((Score[TERRORIST] > Quite) || (Score[CT] > Quite))
                    {
                        gPugTask.Create(E_TASK::SET_STATE, tmDelay + 1.0f, false, STATE_END);
                    }
                    else if ((Score[TERRORIST] == Quite) && (Score[CT] == Quite))
                    {
                        switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                        {
                            case 0: // Vote
                            {
                                break;
                            }
                            case 1: // OT
                            {
                                gPugTask.Create(E_TASK::SET_STATE, tmDelay + 1.0f, false, STATE_HALFTIME);
                                break;
                            }
                            case 2: // End Tied
                            {
                                gPugTask.Create(E_TASK::SET_STATE, tmDelay + 1.0f, false, STATE_END);
                                break;
                            }
                        }
                    }
                    break;
                }
                case STATE_OVERTIME:
                {
                    break;
                }
            }
        }
    }
}

void CPugMod::Status(CBasePlayer *Player)
{
    if (Player)
    {
        auto Players = gPugUtil.GetPlayers(true, true);

        gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 Status: ^3%s^1", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
        gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 Jogadores: ^3%d^1 (Mínimo: %d, Máximo: %d)", gPugCvar.m_Tag->string, Players.size(), static_cast<int>(gPugCvar.m_PlayersMin->value), static_cast<int>(gPugCvar.m_PlayersMax->value));
    }
}

void CPugMod::Scores(CBasePlayer *Player)
{
    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
    {
        auto pEntity = (Player ? Player->edict() : nullptr);

        auto Score = this->GetScore();

        if (Score[TERRORIST] != Score[CT])
        {
            auto Winner = (Score[TERRORIST] > Score[CT]) ? TERRORIST : CT;

            auto Losers = (Score[TERRORIST] > Score[CT]) ? CT : TERRORIST;

            auto Color = (Score[TERRORIST] > Score[CT]) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE;

            if (this->m_State != STATE_END)
            {
                gPugUtil.PrintColor(pEntity, Color, "^4[%s]^1 Os ^3%s^1 estão vencendo: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], Score[Winner], Score[Losers]);
            }
            else
            {
                gPugUtil.PrintColor(pEntity, Color, "^4[%s]^1 Fim de jogo! Os ^3%s^1 venceram a partida: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], Score[Winner], Score[Losers]);
            }
        }
        else
        {
            if (this->m_State != STATE_END)
            {
                gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 O placar está empatado: %d-%d", gPugCvar.m_Tag->string, Score[TERRORIST], Score[CT]);
            }
            else
            {
                gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Fim de jogo! O placar está empatado: %d-%d", gPugCvar.m_Tag->string, Score[TERRORIST], Score[CT]);
            }
        }
    }
    else
    {
        if (Player)
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);
        }
    }
}
