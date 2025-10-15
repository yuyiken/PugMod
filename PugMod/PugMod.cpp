#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    this->m_State = STATE_DEAD;

    this->m_iNumCTWins.fill(0);

    this->m_iNumTerroristWins.fill(0);

    this->m_MapList.clear();

    this->SetState(STATE_DEAD);

    gPugUtil.ServerCommand("exec %s/cfg/maplist.cfg", gPugUtil.GetPath());
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
            this->m_iNumCTWins.fill(0);
            this->m_iNumTerroristWins.fill(0);

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
            this->SetScores(State, 0, 0);

            auto NextState = ((CSGameRules()->m_iNumCTWins + CSGameRules()->m_iNumTerroristWins) < static_cast<int>(gPugCvar.m_Rounds->value)) ? STATE_SECOND_HALF : STATE_OVERTIME;

            this->SwapTeams();

            auto Players = gPugUtil.GetPlayers(true, true);

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
            this->SetScores(State, 0, 0);

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
    if (g_pGameRules)
    {
        CSGameRules()->SwapAllPlayers();

        g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_SvRestart, "1");
    }

    SWAP(this->m_iNumCTWins, this->m_iNumTerroristWins);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Trocando times automaticamente.", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
}

void CPugMod::SetScores(int State, int NumCTWins, int NumTerroristWins)
{
    this->m_iNumCTWins[State] = NumCTWins;
    this->m_iNumTerroristWins[State] = NumTerroristWins;
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
            if (this->m_State == STATE_FIRST_HALF)
            {
                if ((CSGameRules()->m_iNumCTWins + CSGameRules()->m_iNumTerroristWins) == (static_cast<int>(gPugCvar.m_Rounds->value / 2.0f) - 1))
                {
                    gPugUtil.SendHud
                    (
                        Player->edict(),
                        g_Pug_HudParam,
                        "%s\n%s %d : %d %s\nÚLTIMO ROUND",
                        g_Pug_String[this->m_State],
                        g_Pug_TeamShort[TERRORIST],
                        CSGameRules()->m_iNumTerroristWins,
                        CSGameRules()->m_iNumCTWins,
                        g_Pug_TeamShort[CT]
                    );
                    gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");
                    return;
                }
            }
            else if (this->m_State == STATE_SECOND_HALF)
            {
                auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                if ((CSGameRules()->m_iNumCTWins == MaxRounds) || (CSGameRules()->m_iNumCTWins == MaxRounds))
                {
                    gPugUtil.SendHud
                    (
                        Player->edict(),
                        g_Pug_HudParam,
                        "%s\n%s %d : %d %s\nÚLTIMO ROUND",
                        g_Pug_String[this->m_State],
                        g_Pug_TeamShort[TERRORIST],
                        CSGameRules()->m_iNumTerroristWins,
                        CSGameRules()->m_iNumCTWins,
                        g_Pug_TeamShort[CT]
                    );
                    gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");
                    return;
                }
            }
            else if (this->m_State == STATE_OVERTIME)
            {
                /**/
            }

            gPugUtil.SendHud
            (
                Player->edict(),
                g_Pug_HudParam,
                "%s\n%s %d : %d %s",
                g_Pug_String[this->m_State],
                g_Pug_TeamShort[TERRORIST],
                CSGameRules()->m_iNumTerroristWins,
                CSGameRules()->m_iNumCTWins,
                g_Pug_TeamShort[CT]
            );
        }
    }
}


void CPugMod::RestartRound()
{
    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_OVERTIME)
    {
        if (g_pGameRules)
        {
            if (this->m_State == STATE_HALFTIME || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
            {
                if (!CSGameRules()->m_bRoundTerminating)
                {
                    if (CSGameRules()->m_iNumCTWins == 0 && CSGameRules()->m_iNumTerroristWins == 0)
                    {
                        CSGameRules()->m_iNumCTWins = (this->m_iNumCTWins[STATE_FIRST_HALF] + this->m_iNumCTWins[STATE_SECOND_HALF] + this->m_iNumCTWins[STATE_OVERTIME]);

                        CSGameRules()->m_iNumTerroristWins = (this->m_iNumTerroristWins[STATE_FIRST_HALF] + this->m_iNumTerroristWins[STATE_SECOND_HALF] + this->m_iNumTerroristWins[STATE_OVERTIME]);

                        CSGameRules()->UpdateTeamScores();
                        
                        auto Players = gPugUtil.GetPlayers(false, true);

                        for (auto Player : Players)
                        {
                            auto lpInfo = gPugPlayer.GetInfo(Player->entindex());

                            if (lpInfo)
                            {
                                Player->edict()->v.frags = (lpInfo->Stats[STATE_FIRST_HALF].Frags + lpInfo->Stats[STATE_SECOND_HALF].Frags + lpInfo->Stats[STATE_OVERTIME].Frags);

                                Player->m_iDeaths = (lpInfo->Stats[STATE_FIRST_HALF].Deaths + lpInfo->Stats[STATE_SECOND_HALF].Deaths + lpInfo->Stats[STATE_OVERTIME].Deaths);

                                Player->AddPoints(0, true);
                            }
                        }
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
            if (CSGameRules()->m_iNumCTWins || CSGameRules()->m_iNumTerroristWins)
            {
                this->Scores(nullptr);
            }

            gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Iniciado.", gPugCvar.m_Tag->string, CSGameRules()->m_iTotalRoundsPlayed);
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
                this->SetScores(this->m_State, CSGameRules()->m_iNumCTWins, CSGameRules()->m_iNumTerroristWins);
                
                TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

                gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Ganho Por: %s.", gPugCvar.m_Tag->string, CSGameRules()->m_iTotalRoundsPlayed, g_Pug_TeamId[Winner]);

                switch (this->m_State)
                {
                    case STATE_KNIFE_ROUND:
                    {
                        gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, Winner);
                        break;
                    }
                    case STATE_FIRST_HALF:
                    {
                        if ((CSGameRules()->m_iNumCTWins + CSGameRules()->m_iNumTerroristWins) == static_cast<int>(gPugCvar.m_Rounds->value / 2.0f))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                        }
                        break;
                    }
                    case STATE_SECOND_HALF:
                    {
                        auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                        if ((CSGameRules()->m_iNumCTWins > MaxRounds) || (CSGameRules()->m_iNumTerroristWins > MaxRounds))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
                        }
                        else if ((CSGameRules()->m_iNumCTWins == MaxRounds) && (CSGameRules()->m_iNumTerroristWins == MaxRounds))
                        {
                            this->SetScores(STATE_OVERTIME, 0, 0);

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
                        //
                        auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value);

                        //
                        auto MaxRoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value);

                        //
                        auto TotalRounds = (CSGameRules()->m_iNumCTWins + CSGameRules()->m_iNumTerroristWins);

                        //
                        auto OvertimeRound = ((TotalRounds - MaxRounds) % MaxRoundsOT);

                        //
                        auto WinnerCT = ((CSGameRules()->m_iNumCTWins - (MaxRounds / 2)) % MaxRoundsOT) > (MaxRoundsOT / 2);

                        //
                        auto WinnerTR = ((CSGameRules()->m_iNumTerroristWins - (MaxRounds / 2)) % MaxRoundsOT) > (MaxRoundsOT / 2);

                        LOG_CONSOLE(PLID, "[TotalRounds] %d", TotalRounds);
                        LOG_CONSOLE(PLID, "[OvertimeRound] %d", OvertimeRound);
                        LOG_CONSOLE(PLID, "[WinnerCT] %d", WinnerCT ? 1 : 0);
                        LOG_CONSOLE(PLID, "[WinnerCT] %d", WinnerTR ? 1 : 0);

                        if (OvertimeRound == (MaxRoundsOT / 2))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                        }
                        else if(WinnerCT || WinnerTR || !OvertimeRound)
                        {
                            if (CSGameRules()->m_iNumCTWins == CSGameRules()->m_iNumTerroristWins)
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_HALFTIME);
                            }
                            else
                            {
                                gPugTask.Create(E_TASK::SET_STATE, (gPugCvar.m_RoundRestartDelay->value + 1.0f), false, STATE_END);
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
            if (CSGameRules()->m_iNumCTWins != CSGameRules()->m_iNumTerroristWins)
            {
                Sender = (CSGameRules()->m_iNumCTWins > CSGameRules()->m_iNumTerroristWins) ? E_PRINT_TEAM::BLUE : E_PRINT_TEAM::RED;
            }

            if (gPugCvar.m_ScoreText->value)
            {
                if (this->m_State != STATE_END)
                {
                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], CSGameRules()->m_iNumTerroristWins, g_Pug_TeamId[CT], CSGameRules()->m_iNumCTWins);
                }
                else
                {
                    if (!pEntity)
                    {
                        gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                        gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], CSGameRules()->m_iNumTerroristWins, CSGameRules()->m_iNumCTWins, g_Pug_TeamShort[CT]);
                    }

                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo: ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamId[TERRORIST], CSGameRules()->m_iNumTerroristWins, g_Pug_TeamId[CT], CSGameRules()->m_iNumCTWins);
                }
            }
            else
            {
                if (CSGameRules()->m_iNumCTWins != CSGameRules()->m_iNumTerroristWins)
                {
                    auto Winner = (CSGameRules()->m_iNumCTWins > CSGameRules()->m_iNumTerroristWins) ? CT : TERRORIST;
                    auto Losers = (CSGameRules()->m_iNumCTWins > CSGameRules()->m_iNumTerroristWins) ? TERRORIST : CT;

                    auto ScoreWinner = (Winner == CT) ? CSGameRules()->m_iNumCTWins : CSGameRules()->m_iNumTerroristWins;
                    auto ScoreLosers = (Losers == CT) ? CSGameRules()->m_iNumCTWins : CSGameRules()->m_iNumTerroristWins;

                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Os ^3%s^1 estão vencendo: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], ScoreWinner, ScoreLosers);
                    }
                    else
                    {
                        if (!pEntity)
                        {
                            gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                            gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nOs %s venceram\n%s %d : %d %s", g_Pug_TeamId[Winner], g_Pug_TeamShort[TERRORIST], CSGameRules()->m_iNumTerroristWins, CSGameRules()->m_iNumCTWins, g_Pug_TeamShort[CT]);
                        }

                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Os ^3%s^1 venceram: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamId[Winner], ScoreWinner, ScoreLosers);
                    }
                }
                else
                {
                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Placar empatado: %d-%d", gPugCvar.m_Tag->string, CSGameRules()->m_iNumCTWins, CSGameRules()->m_iNumTerroristWins);
                    }
                    else
                    {
                        if (!pEntity)
                        {
                            gPugUtil.ClientCommand(pEntity, "spk \"misc/sheep\"");

                            gPugUtil.SendHud(pEntity, g_Pug_HudParam, "Fim de Jogo!\nPlacar Empatado\n%s %d : %d %s", g_Pug_TeamShort[TERRORIST], CSGameRules()->m_iNumCTWins, CSGameRules()->m_iNumTerroristWins, g_Pug_TeamShort[CT]);
                        }

                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Placar empatado: %d-%d", gPugCvar.m_Tag->string, CSGameRules()->m_iNumCTWins, CSGameRules()->m_iNumTerroristWins);
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
