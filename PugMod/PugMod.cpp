#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
    this->m_State = STATE_DEAD;

    this->m_Score = {};

    this->m_ScoreOT = {};

    this->m_Point = {};

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

    if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_SECOND_OT)
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
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};
            
            gPugTask.Create(E_TASK::SET_STATE, 5.0f, false, STATE_DEATHMATCH);
            break;
        }
        case STATE_DEATHMATCH:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

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
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteMap.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");
            break;
        }
        case STATE_VOTETEAM:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugVoteTeam.Init();

            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "1");
            break;
        }
        case STATE_CAPTAIN:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugLeader.Init();
            break;
        }
        case STATE_KNIFE_ROUND:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};
            
            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Round Faca: O ^3vencedor escolhe o time inicial.", gPugCvar.m_Tag->string);

            gPugLO3.Init();
            break;
        }
        case STATE_FIRST_HALF:
        {
            this->m_Score = {};
            this->m_ScoreOT = {};
            this->m_Point = {};

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

            auto NextState = (this->GetRound() < static_cast<int>(gPugCvar.m_Rounds->value)) ? STATE_SECOND_HALF : STATE_FIRST_OT;

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
                gPugTask.Create(E_TASK::SET_STATE, 3.0f, false, NextState);
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!", gPugCvar.m_Tag->string, g_Pug_String[State]);
            break;
        }
        case STATE_SECOND_HALF:
        {
            this->m_Score[TERRORIST][State] = 0;
            this->m_Score[CT][State] = 0;
            this->m_ScoreOT = {};
            this->m_Point[State] = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_FIRST_OT:
        {
            this->m_ScoreOT = {};
            this->m_Point[State] = {};

            gPugDM.Stop();
            gPugReady.Stop(true);
            gPugTimer.Stop(true);

            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!.", gPugCvar.m_Tag->string, g_Pug_String[State]);

            gPugLO3.Init();
            break;
        }
        case STATE_HALFTIME_OT:
        {
            this->SwapTeams();

            auto Players = gPugUtil.GetPlayers(true, true);

            if (Players.size() < static_cast<size_t>(gPugCvar.m_PlayersMin->value * 2.0f))
            {
                if (gPugCvar.m_ReadyType->value == 1.0f)
                {
                    gPugTimer.Init(STATE_SECOND_OT);
                }
                else if (gPugCvar.m_ReadyType->value == 2.0f)
                {
                    gPugReady.Init(STATE_SECOND_OT);
                }
            }
            else
            {
                gPugTask.Create(E_TASK::SET_STATE, 3.0f, false, STATE_SECOND_OT);
            }
            
            gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Iniciado: Prepare-se !!", gPugCvar.m_Tag->string, g_Pug_String[State]);
            break;
        }
        case STATE_SECOND_OT:
        {
            this->m_Point[State] = {};

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

        CSGameRules()->m_bCompleteReset = true;

        CSGameRules()->RestartRound();
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
    return (this->m_Score[Team][STATE_FIRST_HALF] + this->m_Score[Team][STATE_SECOND_HALF] + this->m_Score[Team][STATE_FIRST_OT] + this->m_Score[Team][STATE_SECOND_OT]);
}

void CPugMod::SetScore(TeamName Team, int State, int Score)
{
    this->m_Score[Team][State] = Score;
}

int CPugMod::GetPoint(int EntityIndex, int Type)
{
    if (EntityIndex >= 1 && EntityIndex <= gpGlobals->maxClients)
    {
        return (this->m_Point[STATE_FIRST_HALF][EntityIndex][Type] + this->m_Point[STATE_SECOND_HALF][EntityIndex][Type] + this->m_Point[STATE_FIRST_OT][EntityIndex][Type] + this->m_Point[STATE_SECOND_OT][EntityIndex][Type]);
    }

    return 0;
}

void CPugMod::SetPoint(int State, int EntityIndex, int Type, int Point)
{
    if (EntityIndex >= 1 && EntityIndex <= gpGlobals->maxClients)
    {
        this->m_Point[State][EntityIndex][Type] = Point;
    }
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
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você já está no time ^3%s^1.", gPugCvar.m_Tag->string, g_Pug_TeamName[Player->m_iTeam]);
        return true;
    }

    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
    {
        if (this->m_State >= STATE_VOTEMAP && this->m_State <= STATE_SECOND_OT)
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
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 O time ^3%s^1 já está completo.", gPugCvar.m_Tag->string, g_Pug_TeamName[Slot]);
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

void CPugMod::DropClient(edict_t *pEntity)
{
    if (this->m_State >= STATE_KNIFE_ROUND && this->m_State <= STATE_SECOND_OT)
    {
        auto Players = gPugUtil.GetPlayers();

        if((Players[TERRORIST].size() < 1) && (Players[CT].size() < 1))
        {
            this->SetState(STATE_END);
        }
        else
        {
            auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

            if (Player)
            {
                if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                {
                    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 ^3%s^1 Saiu da partida..", gPugCvar.m_Tag->string, STRING(pEntity->v.netname));
                }
            }

            auto PlayersMin = static_cast<size_t>(gPugCvar.m_PlayersMin->value - 1.0f);

            if((Players[TERRORIST].size() < PlayersMin) || (Players[CT].size() < PlayersMin))
            {
                gPugVoteEnd.Init();
            }
        }
    }
}

void CPugMod::OnSpawnEquip(CBasePlayer *Player)
{
    if (g_pGameRules)
    {
        if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_FIRST_OT || this->m_State == STATE_SECOND_OT)
        {
            if (CSGameRules()->m_iTotalRoundsPlayed > 0)
            {
                auto ScoreTR = this->GetScore(TERRORIST);
                auto ScoreCT = this->GetScore(CT);

                auto MaxRound = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);
                auto MaxRoundOT = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);
                
                if ((ScoreTR + ScoreCT) == (MaxRound - 1) || (this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == (MaxRoundOT - 1))
                {
                    gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                    gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "%s\n%s %d : %d %s\nÚltimo Round", g_Pug_String[this->m_State], g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                }
                else if ((ScoreTR == MaxRound) || (ScoreCT == MaxRound) || (this->m_ScoreOT[TERRORIST] == MaxRoundOT) || (this->m_ScoreOT[CT] == MaxRoundOT))
                {
                    gPugUtil.ClientCommand(Player->edict(), "spk \"fvox/blip, warning\"");

                    gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "%s\n%s %d : %d %s\nPerigo Fim da Partida", g_Pug_String[this->m_State], g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                }
                else
                {
                    gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "%s\n%s %d - %d %s", g_Pug_String[this->m_State], g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
                }
            }
        }
        else if (this->m_State == STATE_END)
        {
            auto ScoreTR = this->GetScore(TERRORIST);
            auto ScoreCT = this->GetScore(CT);

            gPugUtil.ClientCommand(Player->edict(), "spk \"misc/sheep\"");

            if (ScoreTR == ScoreCT)
            {
                gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "Fim de Jogo!\n%s %d : %d %s\nA Partida terminou empatada", g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT]);
            }
            else
            {
                auto Winner = (ScoreTR > ScoreCT) ? TERRORIST : CT;

                gPugUtil.ClientCommand(Player->edict(), "spk \"misc/sheep\"");

                gPugUtil.SendHud(Player->edict(), g_Pug_HudParam, "Fim de Jogo!\n%s %d : %d %s\nOs %s venceram a partida", g_Pug_TeamNameShort[TERRORIST], ScoreTR, ScoreCT, g_Pug_TeamNameShort[CT], g_Pug_TeamName[Winner]);
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
            this->m_Point[this->m_State] = {};
        }
    }
}

void CPugMod::RoundStart()
{
    if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_FIRST_OT || this->m_State == STATE_SECOND_OT)
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
    if (this->m_State == STATE_KNIFE_ROUND || this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_FIRST_OT || this->m_State == STATE_SECOND_OT)
    {
        if (winStatus == WINSTATUS_CTS || winStatus == WINSTATUS_TERRORISTS)
        {
            if (g_pGameRules)
            {
                TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

                this->m_Score[Winner][this->m_State]++;

                if (this->m_State == STATE_FIRST_OT || this->m_State == STATE_SECOND_OT)
                {
                    this->m_ScoreOT[Winner]++;
                }
                
                gPugUtil.ClientPrint(nullptr, E_PRINT::CONSOLE, "[%s] Round %d Ganho Por: %s.", gPugCvar.m_Tag->string, (this->GetRound() + 1), g_Pug_TeamName[Winner]);

                switch (this->m_State)
                {
                    case STATE_KNIFE_ROUND:
                    {
                        gPugTask.Create(E_TASK::VOTE_SWAP_TEAM, 1.0f, false, Winner);
                        break;
                    }
                    case STATE_FIRST_HALF:
                    {
                        if ((this->m_Score[TERRORIST][this->m_State] + this->m_Score[CT][this->m_State]) == static_cast<int>(gPugCvar.m_Rounds->value / 2.0f))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                        }
                        break;
                    }
                    case STATE_SECOND_HALF:
                    {
                        auto MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value / 2.0f);

                        auto ScoreTR = (this->m_Score[TERRORIST][STATE_FIRST_HALF] + this->m_Score[TERRORIST][STATE_SECOND_HALF]);
                        auto ScoreCT = (this->m_Score[CT][STATE_FIRST_HALF] + this->m_Score[CT][STATE_SECOND_HALF]);

                        if ((ScoreTR > MaxRounds) || (ScoreCT > MaxRounds))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                        }
                        else if ((ScoreTR == MaxRounds) && (ScoreCT == MaxRounds))
                        {
                            switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                            {
                                case 0: // Vote OT
                                {
                                    gPugTask.Create(E_TASK::VOTE_OVERTIME, (tmDelay + 1.0f), false, 0);
                                    break;
                                }
                                case 1: // OT
                                {
                                    gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                                    break;
                                }
                                case 2: // End Tied
                                {
                                    gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    case STATE_FIRST_OT:
                    {
                        auto RoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                        if ((this->m_ScoreOT[TERRORIST] + this->m_ScoreOT[CT]) == RoundsOT)
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME_OT);
                        }
                        break;
                    }
                    case STATE_SECOND_OT:
                    {
                        auto MaxRounds = static_cast<int>(gPugCvar.m_RoundsOT->value / 2.0f);

                        if ((this->m_ScoreOT[TERRORIST] > MaxRounds) || (this->m_ScoreOT[CT] > MaxRounds))
                        {
                            gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
                        }
                        else if ((this->m_ScoreOT[TERRORIST] == MaxRounds) && (this->m_ScoreOT[CT] == MaxRounds))
                        {
                            switch (static_cast<int>(gPugCvar.m_OvertimeType->value))
                            {
                                case 0: // Vote OT
                                {
                                    gPugTask.Create(E_TASK::VOTE_OVERTIME, (tmDelay + 1.0f), false, 0);
                                    break;
                                }
                                case 1: // OT
                                {
                                    gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_HALFTIME);
                                    break;
                                }
                                case 2: // End Tied
                                {
                                    gPugTask.Create(E_TASK::SET_STATE, (tmDelay + 1.0f), false, STATE_END);
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

    gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 Status: ^3%s^1", gPugCvar.m_Tag->string, g_Pug_String[this->m_State]);
    gPugUtil.PrintColor(Player->edict(), Player->entindex(), "^4[%s]^1 TRs: ^3%d^1, CTs: ^3%d^1, SPECs: ^3%d^1", gPugCvar.m_Tag->string, Players[TERRORIST].size(), Players[CT].size(), Players[SPECTATOR].size());
}

void CPugMod::Scores(CBasePlayer *Player)
{
    if (g_pGameRules)
    {
        auto pEntity = Player ? Player->edict() : nullptr;

        if (this->m_State >= STATE_FIRST_HALF && this->m_State <= STATE_END)
        {
            auto ScoreTR = this->GetScore(TERRORIST);
            auto ScoreCT = this->GetScore(CT);

            auto Sender = E_PRINT_TEAM::GREY;

            if (ScoreTR != ScoreCT)
            {
                Sender = (ScoreTR > ScoreCT) ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE;
            }

            if (gPugCvar.m_ScoreText->value)
            {
                if (this->m_State != STATE_END)
                {
                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamName[TERRORIST], ScoreTR, g_Pug_TeamName[CT], ScoreCT);
                }
                else
                {
                    gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo: ^3%s^1 (^4%d^1) - (^4%d^1) ^3%s^1", gPugCvar.m_Tag->string, g_Pug_TeamName[TERRORIST], ScoreTR, g_Pug_TeamName[CT], ScoreCT);
                }
            }
            else
            {
                if (ScoreTR != ScoreCT)
                {
                    auto Winner = (ScoreTR > ScoreCT) ? TERRORIST : CT;
                    auto Losers = (ScoreTR < ScoreCT) ? TERRORIST : CT;

                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Os ^3%s^1 estão vencendo: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamName[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                    else
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Os ^3%s^1 venceram: %d-%d", gPugCvar.m_Tag->string, g_Pug_TeamName[Winner], this->GetScore(Winner), this->GetScore(Losers));
                    }
                }
                else
                {
                    if (this->m_State != STATE_END)
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Placar empatado: %d-%d", gPugCvar.m_Tag->string, ScoreTR, ScoreCT);
                    }
                    else
                    {
                        gPugUtil.PrintColor(pEntity, Sender, "^4[%s]^1 Fim de jogo! Placar empatado: %d-%d", gPugCvar.m_Tag->string, ScoreTR, ScoreCT);
                    }
                }
            }
        }
        else
        {
            gPugUtil.PrintColor(pEntity, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);
        }
    }
}

bool CPugMod::TeamScore(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	if (gPugMod.GetState() >= STATE_FIRST_HALF)
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

	return false;
}

bool CPugMod::ScoreInfo(int msg_dest, int msg_type, const float *pOrigin, edict_t *pEntity)
{
    auto State = gPugMod.GetState();

    if (State >= STATE_FIRST_HALF && State <= STATE_SECOND_OT)
    {
        if (gPugCvar.m_ScorePlayers->value)
        {
            auto EntityIndex = gPugEngine.GetByte(0);

            if (EntityIndex >= 1 && EntityIndex <= gpGlobals->maxClients)
            {
                gPugMod.SetPoint(State, EntityIndex, 0, gPugEngine.GetShort(1));
                gPugMod.SetPoint(State, EntityIndex, 1, gPugEngine.GetShort(2));

                gPugEngine.SetArgInt(1, gPugMod.GetPoint(EntityIndex, 0));
                gPugEngine.SetArgInt(2, gPugMod.GetPoint(EntityIndex, 1));
            }
        }
    }

    return false;
}