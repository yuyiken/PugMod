#include "precompiled.h"
#include "PugMod.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    this->m_State = STATE_DEAD;

    this->m_Score.fill({});

    this->m_ScoreOT.fill(0);

    this->m_MapList.clear();

    this->SetState(STATE_DEAD);

    gPugUtil.ServerCommand("exec %s/cfg/maplist.cfg", gPugUtil.GetPath());

    gPugEngine.RegisterHook("TeamScore", this->TeamScore);

    gPugEngine.RegisterHook("ScoreInfo", this->ScoreInfo);
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

void CPugMod::AddMap(const char* Map)
{
    if (Map)
    {
        if (Map[0u] != '\0')
        {
            char *MapName = strdup(Map);

            if (g_engfuncs.pfnIsMapValid(MapName))
            {
                this->m_MapList.insert(std::make_pair(this->m_MapList.size(), MapName));
            }

            free(MapName);
        }
    }
}

std::map<int, std::string> CPugMod::GetMaps()
{
    return this->m_MapList;
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
            gPugTask.Create(E_TASK::SET_STATE, 5.0f, false, STATE_DEATHMATCH);
            break;
        }
        case STATE_DEATHMATCH:
        {
            if (gPugCvar.m_DM_Enable->value)
            {
                gPugDM.Init();
            }

            if (gPugCvar.m_ReadyType->value == 1.0f)
            {
                gPugTimer.Init(gPugCvar.m_VoteMap->value ? STATE_VOTEMAP : STATE_VOTETEAM);
            }
            else if (gPugCvar.m_ReadyType->value == 2.0f)
            {
                gPugReady.Init(gPugCvar.m_VoteMap->value ? STATE_VOTEMAP : STATE_VOTETEAM);
            }
            
            break;
        }
        case STATE_VOTEMAP:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteMap.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");
            break;
        }
        case STATE_VOTETEAM:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteTeam.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");
            break;
        }
        case STATE_CAPTAIN:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugLeader.Init();
            break;
        }
        case STATE_KNIFE_ROUND:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Round Faca: O ^3vencedor escolhe o time inicial.", gPugCvar.m_Tag->string);

            gPugLO3.Init();
            break;
        }
        case STATE_FIRST_HALF:
        {
            this->m_Score.fill({});
            this->m_ScoreOT.fill(0);

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);
            
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

            if (Players.size() < static_cast<size_t>(gPugCvar.m_PlayersMin->value * 2.0f))
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
            else
            {
                gPugTask.Create(E_TASK::SET_STATE, 4.0f, false, NextState);
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!", gPugCvar.m_Tag->string, g_Pug_String[State]);
            break;
        }
        case STATE_SECOND_HALF:
        {
            this->m_Score[TERRORIST][STATE_SECOND_HALF] = 0;
            this->m_Score[CT][STATE_SECOND_HALF] = 0;

            this->m_ScoreOT.fill(0);

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_OVERTIME:
        {
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_END:
        {
            this->Scores(nullptr);

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
		if (this->GetScore(TERRORIST) == this->GetScore(CT))
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

int CPugMod::GetRound()
{
    return (this->GetScore(TERRORIST) + this->GetScore(CT));
}

int CPugMod::GetScore(TeamName Team)
{
    return (this->m_Score[Team][STATE_FIRST_HALF] + this->m_Score[Team][STATE_SECOND_HALF] + this->m_Score[Team][STATE_OVERTIME]);
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
            if (Players[SPECTATOR].size() < 1)
            {
                gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_END);
            }
        }
    }
}

void CPugMod::GiveDefaultItems(CBasePlayer *Player)
{
    if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_iTotalRoundsPlayed > 0)
            {
                if (this->m_State == STATE_FIRST_HALF)
                {
                    if ((this->GetScore(TERRORIST) + this->GetScore(CT)) == (static_cast<int>(gPugCvar.m_Rounds->value / 2.0f) - 1))
                    {
                        gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "PRIMEIRO TEMPO\nÚLTIMO ROUND\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                    else
                    {
                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "PRIMEIRO TEMPO\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                }
                else if (this->m_State == STATE_SECOND_HALF)
                {
                    auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                    if ((this->GetScore(TERRORIST) == MaxRounds) || (this->GetScore(CT) == MaxRounds))
                    {
                        gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "SEGUNDO TEMPO\nPERIGO: FIM DA PARTIDA\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                    else
                    {
                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "SEGUNDO TEMPO\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                }
                else if (this->m_State == STATE_OVERTIME)
                {
                    auto RoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                    if ((this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == (RoundsOT - 1))
                    {
                        gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "PRIMEIRO OVERTIME\nÚLTIMO ROUND\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                    else if ((this->m_ScoreOT[TERRORIST] == RoundsOT) || (this->m_ScoreOT[CT] == RoundsOT))
                    {
                        gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "SEGUNDO OVERTIME\nPERIGO: FIM DA PARTIDA\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                    else
                    {
                        gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "OVERTIME\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }
                }
            }
        }
    }
}

void CPugMod::RoundStart()
{
    if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (this->GetScore(TERRORIST) || this->GetScore(CT))
            {
                this->Scores(nullptr);
            }

            gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Iniciado.", gPugCvar.m_Tag->string, this->GetRound() + 1);
        }
    }
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
    {
        if (winStatus == WINSTATUS_CTS || winStatus == WINSTATUS_TERRORISTS)
        {
            if (g_pGameRules)
            {
                TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

                this->m_Score[Winner][this->m_State]++;

                this->m_ScoreOT[Winner]++;
                
                gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Ganho Por: %s.", gPugCvar.m_Tag->string, (this->GetRound() + 1), g_Pug_TeamId[Winner]);

                switch (this->m_State)
                {
                    case STATE_KNIFE_ROUND:
                    {
                        gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, Winner);
                        break;
                    }
                    case STATE_FIRST_HALF:
                    {
                        if ((this->GetScore(TERRORIST) + this->GetScore(CT)) == static_cast<int>(gPugCvar.m_Rounds->value / 2.0f))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                        }
                        break;
                    }
                    case STATE_SECOND_HALF:
                    {
                        auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                        if ((this->GetScore(TERRORIST) > MaxRounds) || (this->GetScore(CT) > MaxRounds))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
                        }
                        else if ((this->GetScore(TERRORIST) == MaxRounds) && (this->GetScore(CT) == MaxRounds))
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
                            }
                        }
                        break;
                    }
                    case STATE_OVERTIME:
                    {
                        auto RoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                        if ((this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == RoundsOT)
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                        }
                        else if ((this->m_ScoreOT[TERRORIST] > RoundsOT) || (this->m_ScoreOT[CT] > RoundsOT))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
                        }
                        else if ((this->m_ScoreOT[TERRORIST] == RoundsOT) && (this->m_ScoreOT[CT] == RoundsOT))
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
                            }
                        }

                        break;
                    }
                }   
            }
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
    if (g_pGameRules)
    {
        auto pEntity = Player ? Player->edict() : nullptr;

        auto Sender = E_PRINT_TEAM::GREY;

        if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
        {
            if (this->GetScore(TERRORIST) != this->GetScore(CT))
            {
                Sender = (this->GetScore(TERRORIST) > this->GetScore(CT)) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE;
            }

            if (gPugCvar.m_ScoreText->value)
            {
                if (this->m_State != STATE_END)
                {
                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], this->GetScore(TERRORIST), g_Pug_TeamId[CT], this->GetScore(CT));
                }
                else
                {
                    if (!pEntity)
                    {
                        gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                        gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                    }

                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo: ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], this->GetScore(TERRORIST), g_Pug_TeamId[CT], this->GetScore(CT));
                }
            }
            else
            {
                if (this->GetScore(TERRORIST) != this->GetScore(CT))
                {
                    auto Winner = (this->GetScore(TERRORIST) > this->GetScore(CT)) ? TERRORIST : CT;
                    auto Losers = (this->GetScore(TERRORIST) < this->GetScore(CT)) ? TERRORIST : CT;

                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Os ^3%s^1 estão vencendo: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                    else
                    {
                        if (!pEntity)
                        {
                            gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                            gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nOs %s venceram\n%s %d : %d %s", g_Pug_TeamId[Winner], g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                        }

                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Os ^3%s^1 venceram: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                }
                else
                {
                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Placar empatado: %d-%d", gPugCvar.m_Tag->string, this->GetScore(TERRORIST), this->GetScore(CT));
                    }
                    else
                    {
                        if (!pEntity)
                        {
                            gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                            gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nPlacar Empatado\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], this->GetScore(TERRORIST), this->GetScore(CT), g_Pug_TeamShort[CT]);
                        }

                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Placar empatado: %d-%d", gPugCvar.m_Tag->string, this->GetScore(TERRORIST), this->GetScore(CT));
                    }
                }
            }
        }
        else
        {
            gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);
        }
    }
}

bool CPugMod::TeamScore(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	if (gPugMod.GetState() >= STATE_HALFTIME)
	{
		if (gPugCvar.m_ScoreTeams)
		{
			if (gPugCvar.m_ScoreTeams->value > 0.0f)
			{
				auto Team = gPugEngine.GetString(0);

				if (Team)
				{
					if (Team[0u] == 'T')
					{
						gPugEngine.SetArgInt(1, gPugMod.GetScore(TERRORIST));
					}
					else if (Team[0u] == 'C')
					{
						gPugEngine.SetArgInt(1, gPugMod.GetScore(CT));
					}
				}
			}
		}
	}

	return false;
}

bool CPugMod::ScoreInfo(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	if (gPugMod.GetState() >= STATE_HALFTIME)
	{
		if (gPugCvar.m_ScorePlayers)
		{
			if (gPugCvar.m_ScorePlayers->value > 0.0f)
			{
				auto Player = UTIL_PlayerByIndexSafe(gPugEngine.GetByte(0));

		        if (Player)
		        {
		            auto lpInfo = gPugPlayer.GetInfo(Player->entindex());

		            if (lpInfo)
		            {
		                gPugEngine.SetArgInt(1, (int)(lpInfo->Stats[STATE_FIRST_HALF].Frags + lpInfo->Stats[STATE_SECOND_HALF].Frags + lpInfo->Stats[STATE_OVERTIME].Frags));

		                gPugEngine.SetArgInt(2, lpInfo->Stats[STATE_FIRST_HALF].Deaths + lpInfo->Stats[STATE_SECOND_HALF].Deaths + lpInfo->Stats[STATE_OVERTIME].Deaths);
		            }
		        }
			}
		}
	}

	return false;
}