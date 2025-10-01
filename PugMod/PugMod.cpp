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

    switch(State)
    {
        case STATE_DEAD:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugTask.Create(E_TASK::SET_STATE, 5.0f, false, STATE_DEATHMATCH);
            break;
        }
        case STATE_DEATHMATCH:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            auto NextState = (gPugCvar.m_VoteMap->value ? STATE_VOTEMAP : STATE_VOTETEAM);

            if (gPugCvar.m_ReadyType->value == 1.0f)
            {
                gPugTimer.Init(NextState);
            }
            else if (gPugCvar.m_ReadyType->value == 2.0f)
            {
                gPugReady.Init(NextState);
            }

            if (gPugCvar.m_DM_Enable->value)
            {
                gPugDM.Init();
            }
            
            break;
        }
        case STATE_VOTEMAP:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugDM.Stop();

            gPugVoteMap.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");
            break;
        }
        case STATE_VOTETEAM:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugDM.Stop();

            gPugVoteTeam.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");
            break;
        }
        case STATE_CAPTAIN:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugLeader.Init();
            break;
        }
        case STATE_KNIFE_ROUND:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Round Faca: O ^3vencedor escolhe o time inicial.", gPugCvar.m_Tag->string);

            gPugLO3.Init();
            break;
        }
        case STATE_FIRST_HALF:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);
            
            if (g_pGameRules)
            {
                CSGameRules()->BalanceTeams();
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_HALFTIME:
        {
            this->SwapTeams();

            auto Players = gPugUtil.GetPlayers(true, true);

            auto NextState = (this->GetRound() < static_cast<int>(gPugCvar.m_Rounds->value)) ? STATE_SECOND_HALF : STATE_OVERTIME;

            if (Players.size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value * 2.0f))
            {
                gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, NextState);
            }
            else
            {
                if (gPugCvar.m_ReadyType->value == 1.0f)
                {
                    gPugTimer.Init(NextState);
                }
                else if (gPugCvar.m_ReadyType->value == 2.0f)
                {
                    gPugReady.Init(NextState);
                }
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!", gPugCvar.m_Tag->string, g_Pug_String[State]);
            break;
        }
        case STATE_SECOND_HALF:
        {
            this->m_Score[TERRORIST][State] = 0;
            this->m_Score[CT][State] = 0;

            this->m_ScoreOT.fill(0);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_OVERTIME:
        {
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_END:
        {
            this->Scores(nullptr);

            this->SendHudMessage();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_LastMap, STRING(gpGlobals->mapname));

            auto Players = gPugUtil.GetPlayers(true, true);

            if (Players.size() >= static_cast<size_t>(gPugCvar.m_PlayersMin->value))
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_VOTEMAP);
            }
            else
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_DEATHMATCH);
            }

            break;
        }
    }

    gPugUtil.ServerCommand("exec %s/cfg/%s.cfg", gPugUtil.GetPath(), g_Pug_Config[State]);

    return this->m_State;
}

void CPugMod::SwapTeams()
{
	if (this->GetRound() >= static_cast<int>(gPugCvar.m_Rounds->value))
	{
        auto Score = this->GetScore();

		if (Score[TERRORIST] == Score[CT])
		{
			this->m_ScoreOT.fill(0);
            return;
		}
	}
    
    if (g_pGameRules)
    {
        CSGameRules()->SwapAllPlayers();

        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_SvRestart, "1");
    }

    SWAP(this->m_Score[TERRORIST], this->m_Score[CT]);

    SWAP(this->m_ScoreOT[TERRORIST], this->m_ScoreOT[CT]);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Trocando times automaticamente.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
}

std::array<int, SPECTATOR + 1> CPugMod::GetScore()
{
    std::array<int, SPECTATOR + 1> Score = {0};

    Score[TERRORIST] = this->m_Score[TERRORIST][STATE_FIRST_HALF] + this->m_Score[TERRORIST][STATE_SECOND_HALF] + this->m_Score[TERRORIST][STATE_OVERTIME];

    Score[CT] = this->m_Score[CT][STATE_FIRST_HALF] + this->m_Score[CT][STATE_SECOND_HALF] + this->m_Score[CT][STATE_OVERTIME];

    return Score;
}

int CPugMod::GetRound()
{
    auto Score = this->GetScore();

    return (Score[TERRORIST] + Score[CT] + 1);
}

int CPugMod::GetWinner()
{
    auto Score = this->GetScore();

    if (Score[TERRORIST] != Score[CT])
    {
        if (Score[TERRORIST] > Score[CT])
        {
            return TERRORIST;
        }
        else
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

		if (Players[Slot].size() >= static_cast<size_t>(gPugCvar.m_PlayersMax->value))
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
                if (Player->m_iLastAccount < 1)
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

        auto PlayersMin = static_cast<size_t>(gPugCvar.m_PlayersMin->value - 1.0f);

        if((Players[TERRORIST].size() < PlayersMin) || (Players[CT].size() < PlayersMin))
        {
            if (Players[SPECTATOR].size() < 2)
            {
                gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_END);
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
            this->m_Score[TERRORIST][this->m_State] = 0;
            this->m_Score[CT][this->m_State] = 0;
        }
        else
        {
            if (CSGameRules()->m_iNumTerroristWins || CSGameRules()->m_iNumCTWins)
            {
                gPugTask.Create(E_TASK::ROUND_START_HUD, 1.0f, false, 0);
            }
        }
    }
}

void CPugMod::RoundStart()
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_iNumTerroristWins || CSGameRules()->m_iNumCTWins)
            {
                this->Scores(nullptr);
            }
        }
        
        gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Iniciado.", gPugCvar.m_Tag->string, this->GetRound());
    }
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (winStatus == WINSTATUS_CTS || winStatus == WINSTATUS_TERRORISTS)
        {
            TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

            this->m_Score[Winner][this->m_State] += 1;

			if (this->m_State == STATE_OVERTIME)
			{
				this->m_ScoreOT[Winner] += 1;
			}

            gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Ganho Por: %s.", gPugCvar.m_Tag->string, this->GetRound(), g_Pug_TeamId[Winner]);

            this->CheckScore();
        }
    }
}

void CPugMod::CheckScore()
{
    auto Score = this->GetScore();

    switch (this->m_State)
    {
        case STATE_KNIFE_ROUND:
        {
            gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, this->GetWinner());
            break;
        }
        case STATE_FIRST_HALF:
        {
            auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

            if ((Score[TERRORIST] + Score[CT]) == MaxRounds)
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
            }
            break;
        }
        case STATE_SECOND_HALF:
        {
            auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

            if ((Score[TERRORIST] > MaxRounds) || (Score[CT] > MaxRounds))
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
            }
            else if ((Score[TERRORIST] == MaxRounds) && (Score[CT] == MaxRounds))
            {
                switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                {
                    case 0: // Vote OT
                    {
                        gPugTask.Create(E_TASK::VOTE_OVERTIME, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, 0);
                        break;
                    }
                    case 1: // OT
                    {
                        gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                        break;
                    }
                    case 2: // End Tied
                    {
                        gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
                        break;
                    }
                    default: // Sudden Death Round: Nothing to do
                    {
                        break;
                    }
                }
            }
            break;
        }
        case STATE_OVERTIME:
        {
            auto MaxRounds = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

            if (this->m_ScoreOT[TERRORIST] > MaxRounds || this->m_ScoreOT[CT] > MaxRounds)
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
            }
            else if (((this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) % MaxRounds) == 0)
            {
                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
            }

            break;
        }
    }
}

void CPugMod::Status(CBasePlayer *Player)
{
    auto Players = gPugUtil.GetPlayers();

    if (Player)
    {
        gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 Status: ^3%s^1", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
        gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 TRs: ^3%d^1, CTs: ^3%d^1, Espectadores: ^3%d^1", gPugCvar.m_Tag->string, Players[TERRORIST].size(), Players[CT].size(), Players[SPECTATOR].size());
    }
}

void CPugMod::Scores(CBasePlayer *Player)
{
	auto pEntity = Player ? Player->edict() : nullptr;

    auto Sender = E_PRINT_TEAM::GREY;

	if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
	{
        auto Score = this->GetScore();

		if (Score[TERRORIST] != Score[CT])
		{
			Sender = (Score[TERRORIST] > Score[CT]) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE;
		}

		if (gPugCvar.m_ScoreText->value)
		{
			if (this->m_State != STATE_END)
			{
				gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], Score[TERRORIST], g_Pug_TeamId[CT], Score[CT]);
			}
			else
			{
                if (!pEntity)
                {
                    gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                    gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], Score[TERRORIST], Score[CT], g_Pug_TeamShort[CT]);
                }

				gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo: ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], Score[TERRORIST], g_Pug_TeamId[CT], Score[CT]);
			}
		}
		else
		{
			if (Score[TERRORIST] != Score[CT])
			{
				int Winner = (Score[TERRORIST] > Score[CT]) ? TERRORIST : CT;

				int Losers = (Score[TERRORIST] > Score[CT]) ? CT : TERRORIST;

				if (this->m_State != STATE_END)
				{
					gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Os ^3%s^1 estão vencendo: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], Score[Winner], Score[Losers]);
				}
				else
				{
                    if (!pEntity)
                    {
                        gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                        gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nOs %s venceram\n%s %d : %d %s", g_Pug_TeamId[Winner], g_Pug_TeamShort[TERRORIST], Score[TERRORIST], Score[CT], g_Pug_TeamShort[CT]);
                    }

					gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Os ^3%s^1 venceram: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], Score[Winner], Score[Losers]);
				}
			}
			else
			{
				if (this->m_State != STATE_END)
				{
					gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Placar empatado: %d-%d", gPugCvar.m_Tag->string, Score[TERRORIST], Score[CT]);
				}
				else
				{
                    if (!pEntity)
                    {
                        gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                        gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nPlacar Empatado\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->m_Score[TERRORIST], this->m_Score[CT], g_Pug_TeamShort[CT]);
                    }

					gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Placar empatado: %d-%d", gPugCvar.m_Tag->string, this->m_Score[TERRORIST], this->m_Score[CT]);
				}
			}
		}
	}
	else
	{
		gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);
	}
}

void CPugMod::SendHudMessage()
{
    if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        auto Score = this->GetScore();
        
        auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f - 1.0f);

        switch (this->m_State)
        {
            case STATE_FIRST_HALF:
            {
                if (this->GetRound() == MaxRounds)
                {
                    gPugUtil.ClientCommand(nullptr, "spk \"fvox/blip, warning\"");
                    gPugUtil.SendHud(nullptr, g_Pug_HudParam, "%s\n%s %d : %d %s\nÚLTIMO ROUND", g_Pug_String[this->m_State], g_Pug_TeamShort[TERRORIST], Score[TERRORIST], Score[CT], g_Pug_TeamShort[CT]);
                    return;
                }
                break;
            }
            case STATE_SECOND_HALF:
            {
                if ((Score[TERRORIST] == MaxRounds) || (Score[CT] == MaxRounds))
                {
                    gPugUtil.ClientCommand(nullptr, "spk \"fvox/blip, warning\"");
                    gPugUtil.SendHud(nullptr, g_Pug_HudParam, "%s\n%s %d : %d %s\nÚLTIMO ROUND", g_Pug_String[this->m_State], g_Pug_TeamShort[TERRORIST], Score[TERRORIST], Score[CT], g_Pug_TeamShort[CT]);
                    return;
                }
                break;
            }
            case STATE_OVERTIME:
            {
                break;
            }
        }

        gPugUtil.SendHud(nullptr, g_Pug_HudParam, "%s\n%s %d : %d %s", g_Pug_String[this->m_State], g_Pug_TeamShort[TERRORIST], Score[TERRORIST], Score[CT], g_Pug_TeamShort[CT]);
    }
}