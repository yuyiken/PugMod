#include "precompiled.h"

CPugStats gPugStats;

void CPugStats::ServerActivate()
{
    // State
    this->m_State = STATE_DEAD;

	// Match
	this->m_Match.Reset();

	// Player
	this->m_Player.clear();

	// Round events
	this->m_RoundEvent.clear();

	// Register Say Text messages
	gPugEngine.RegisterHook("SayText", this->SayText);
}

void CPugStats::SetState(int State)
{
	// Update State
	this->m_State = State;

	// Switch
    switch (State)
    {
        case STATE_DEAD:
        {
            // Clear match data
            this->m_Match.Reset();

            // Clear player data
            this->m_Player.clear();

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_DEATHMATCH:
        case STATE_VOTEMAP:
        case STATE_VOTETEAM:
        case STATE_CAPTAIN:
		{
			// Clear match data
			this->m_Match.Reset();

			// Loop each player
			for (auto& Player : this->m_Player)
			{
				// Clear Chat Log
				Player.second.ChatLog.clear();

				// Clear player round stats
				Player.second.Round.Reset();
			}

			// Clear event data
			this->m_RoundEvent.clear();
			break;
		}
        case STATE_KNIFE_ROUND:
        {
            // Clear match data
            this->m_Match.Reset();

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_FIRST_HALF:
        {
            // Set start time
            this->m_Match.StartTime = time(0);
            
            // Set end time
            this->m_Match.EndTime = 0;
            
            // Set hostname
            this->m_Match.HostName = g_engfuncs.pfnCVarGetString("hostname");
            
            // Set map name
            this->m_Match.Map = STRING(gpGlobals->mapname);
            
            // Set address
            this->m_Match.Address = g_engfuncs.pfnCVarGetString("net_address");
            
            // Set score
            this->m_Match.Score.fill(0);
            
            // Set winner
            this->m_Match.Winner = 0;
            
            // Set rounds played
            this->m_Match.Rounds = 0;

            // Number of rounds in match
            this->m_Match.MaxRounds = static_cast<int>(gPugCvar.m_Rounds->value);

            // Number of overtime rounds
            this->m_Match.MaxRoundsOT = static_cast<int>(gPugCvar.m_RoundsOT->value);

            // Match Game Mode
            this->m_Match.GameMode = static_cast<int>(gPugCvar.m_PlayGameMode->value);

            // Match will have Knife Round
            this->m_Match.KnifeRound = static_cast<int>(gPugCvar.m_PlayKnifeRound->value);
            
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[this->m_State].Reset();

                // Clear Chat Log
                Player.second.ChatLog.clear();
            
                // Clear player round stats
                Player.second.Round.Reset();
            }

            // Clear event data
            this->m_RoundEvent.clear();
            break;
        }
        case STATE_HALFTIME:
        {
			if (gPugMod.GetRound() >= static_cast<int>(gPugCvar.m_Rounds->value))
			{
				if (gPugMod.GetScore(TERRORIST) == gPugMod.GetScore(CT))
				{
					return;
				}
			}

			this->m_Match.SwapScores();
            break;
        }
        case STATE_SECOND_HALF:
        {
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[this->m_State].Reset();

                // Clear player round stats
                Player.second.Round.Reset();
            }
            break;
        }
        case STATE_OVERTIME:
        {
            // Loop each player
            for (auto& Player : this->m_Player)
            {
                // Clear player stats
                Player.second.Stats[this->m_State].Reset();

                // Clear player round stats
                Player.second.Round.Reset();
            }
            break;
        }
        case STATE_END:
        {
            // Set end time
            this->m_Match.EndTime = time(0);

            // Export data
            this->ExportData();
            break;
        }
    }
}

const char* CPugStats::GetAuthId(CBasePlayer *Player)
{
    if (Player)
    {
        if (!FNullEnt(Player->edict()))
        {
            if (!(Player->edict()->v.flags & FL_FAKECLIENT))
            {
                return g_engfuncs.pfnGetPlayerAuthId(Player->edict());
            }

            return STRING(Player->edict()->v.netname);
        }
    }

    return "";
}

void CPugStats::GetIntoGame(CBasePlayer *Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].JoinGameTime = time(0);

		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;

		if (!Player->IsBot())
		{
			LOG_CONSOLE(PLID, "[%s] %s: %d", __func__, STRING(Player->edict()->v.netname), Player->m_iTeam);
		}
	}
}

void CPugStats::DropClient(edict_t *pEntity)
{
    auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

    if (Player)
    {
        auto Auth = this->GetAuthId(Player);

        if (Auth)
        {
            this->m_Player[Auth].DisconnectTime = time(0);
        }
    }
}

void CPugStats::ChooseTeam(CBasePlayer* Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;
	}
}

void CPugStats::SwitchTeam(CBasePlayer* Player)
{
	auto Auth = this->GetAuthId(Player);

	if (Auth)
	{
		this->m_Player[Auth].Name = STRING(Player->edict()->v.netname);

		this->m_Player[Auth].Team = static_cast<int>(Player->m_iTeam);

		this->m_Player[Auth].IsBot = Player->IsBot() ? 1 : 0;
	}
}

void CPugStats::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if ((playerAnim == PLAYER_ATTACK1) || (playerAnim == PLAYER_ATTACK2))
		{
			if (Player->m_pActiveItem)
			{
				if (Player->m_pActiveItem->m_iId != WEAPON_NONE)
				{
					if (Player->m_pActiveItem->m_iId != WEAPON_HEGRENADE && Player->m_pActiveItem->m_iId != WEAPON_C4 && Player->m_pActiveItem->m_iId != WEAPON_SMOKEGRENADE && Player->m_pActiveItem->m_iId != WEAPON_FLASHBANG && Player->m_pActiveItem->m_iId != WEAPON_SHIELDGUN)
					{
						auto Auth = this->GetAuthId(Player);

						if (Auth)
						{
							this->m_Player[Auth].Stats[this->m_State].Shots++;

							this->m_Player[Auth].Stats[this->m_State].Weapon[Player->m_pActiveItem->m_iId].Shots++;

							this->m_Player[Auth].Round.Shots++;
						}
					}
				}
			}
		}
	}
}

void CPugStats::TakeDamage(CBasePlayer* Victim, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (!Victim->m_bKilledByBomb)
		{
			auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

			if (Attacker)
			{
				if (Victim->entindex() != Attacker->entindex())
				{
					if (Victim->IsPlayer() && Attacker->IsPlayer())
					{
						if (CSGameRules()->FPlayerCanTakeDamage(Victim, Attacker))
						{
							auto VictimAuth = this->GetAuthId(Victim);

							auto AttackerAuth = this->GetAuthId(Attacker);

							auto DamageTaken = (int)(Victim->m_iLastClientHealth - clamp(Victim->edict()->v.health, 0.0f, Victim->edict()->v.health));

							auto ItemIndex = (bitsDamageType & DMG_EXPLOSION) ? WEAPON_HEGRENADE : ((Attacker->m_pActiveItem) ? Attacker->m_pActiveItem->m_iId : WEAPON_NONE);

							this->m_Player[AttackerAuth].Stats[this->m_State].Hits++;
							this->m_Player[AttackerAuth].Stats[this->m_State].Damage += DamageTaken;

							this->m_Player[AttackerAuth].Stats[this->m_State].HitBox[Victim->m_LastHitGroup][0]++;
							this->m_Player[AttackerAuth].Stats[this->m_State].HitBox[Victim->m_LastHitGroup][1] += DamageTaken;

							this->m_Player[AttackerAuth].Stats[this->m_State].Weapon[ItemIndex].Hits++;
							this->m_Player[AttackerAuth].Stats[this->m_State].Weapon[ItemIndex].Damage += DamageTaken;

							this->m_Player[VictimAuth].Stats[this->m_State].HitsReceived++;
							this->m_Player[VictimAuth].Stats[this->m_State].DamageReceived += DamageTaken;

							this->m_Player[VictimAuth].Stats[this->m_State].HitBox[Victim->m_LastHitGroup][2]++;
							this->m_Player[VictimAuth].Stats[this->m_State].HitBox[Victim->m_LastHitGroup][3] += DamageTaken;

							this->m_Player[VictimAuth].Stats[this->m_State].Weapon[ItemIndex].HitsReceived++;
							this->m_Player[VictimAuth].Stats[this->m_State].Weapon[ItemIndex].DamageReceived += DamageTaken;

							this->m_Player[AttackerAuth].Round.Hits++;
							this->m_Player[AttackerAuth].Round.Damage += DamageTaken;

							this->m_Player[VictimAuth].Round.HitsReceived++;
							this->m_Player[VictimAuth].Round.DamageReceived += DamageTaken;

							this->m_Player[AttackerAuth].Round.PlayerDamage[VictimAuth] += DamageTaken;
						}
					}
				}
			}
		}
	}
}

void CPugStats::PlayerKilled(CBasePlayer* Victim, entvars_t* pevKiller, entvars_t* pevInflictor)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (!Victim->m_bKilledByBomb)
		{
			auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevKiller));

			if (Killer)
			{
				if (Killer->IsPlayer() && Victim->IsPlayer())
				{
					auto VictimIndex = Victim->entindex();

					auto KillerIndex = Killer->entindex();

					auto ItemIndex = (Victim->m_bKilledByGrenade) ? WEAPON_HEGRENADE : ((Killer->m_pActiveItem) ? Killer->m_pActiveItem->m_iId : WEAPON_NONE);

					auto VictimAuth = this->GetAuthId(Victim);

					auto KillerAuth = this->GetAuthId(Killer);

					if (VictimIndex != KillerIndex)
					{
						this->m_Player[KillerAuth].Stats[this->m_State].Frags++;

						this->m_Player[KillerAuth].Stats[this->m_State].Weapon[ItemIndex].Frags++;

						this->m_Player[VictimAuth].Stats[this->m_State].Deaths++;

						this->m_Player[VictimAuth].Stats[this->m_State].Weapon[ItemIndex].Deaths++;

						this->m_Player[KillerAuth].Round.Frags++;

						this->m_Player[VictimAuth].Round.Deaths++;

						if ((gpGlobals->time - this->m_Player[KillerAuth].Round.KillTime) < 0.25f)
						{
							this->m_Player[KillerAuth].Stats[this->m_State].DoubleKill++;
						}

						this->m_Player[KillerAuth].Round.KillTime = gpGlobals->time;

						if (Victim->m_LastHitGroup == 1)
						{
							this->m_Player[KillerAuth].Stats[this->m_State].Headshots++;

							this->m_Player[KillerAuth].Stats[this->m_State].Weapon[ItemIndex].Headshots++;

							this->m_Player[KillerAuth].Round.Headshots++;
						}

						for (int i = 1; i <= gpGlobals->maxClients; ++i)
						{
							auto Player = UTIL_PlayerByIndexSafe(i);

							if (Player)
							{
								if (!Player->IsDormant())
								{
									if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
									{
										if (Player->entindex() != KillerIndex)
										{
											auto Auth = this->GetAuthId(Player);

											if (Auth)
											{
												if (this->m_Player[Auth].Round.PlayerDamage[VictimAuth] >= static_cast<int>(gPugCvar.m_ST_AssistanceDmg->value))
												{
													this->m_Player[Auth].Stats[this->m_State].Assists++;
												}
											}
										}
									}
								}
							}
						}

						if (!Victim->m_bKilledByGrenade)
						{
							if (Killer->IsBlind())
							{
								this->m_Player[KillerAuth].Stats[this->m_State].BlindFrags++;
							}

							if (Victim->IsBlind())
							{
								this->m_Player[VictimAuth].Stats[this->m_State].BlindDeaths++;
							}
						}

						if (ItemIndex != WEAPON_AWP)
						{
							if (Victim->m_iLastClientHealth >= 100)
							{
								if (Killer->m_lastDamageAmount >= 100)
								{
									this->m_Player[KillerAuth].Stats[this->m_State].OneShot++;
								}
							}
						}

						if (ItemIndex == WEAPON_AWP || ItemIndex == WEAPON_SCOUT || ItemIndex == WEAPON_G3SG1 || ItemIndex == WEAPON_SG550)
						{
							if (Killer->m_iClientFOV == DEFAULT_FOV)
							{
								this->m_Player[KillerAuth].Stats[this->m_State].NoScope++;
							}
						}

						if (!(Victim->edict()->v.flags & FL_ONGROUND))
						{
							if (Victim->m_flFallVelocity > 0.0f)
							{
								this->m_Player[KillerAuth].Stats[this->m_State].FlyFrags++;
							}
						}

						if (ItemIndex != WEAPON_HEGRENADE)
						{
							if (!gPugUtil.IsPlayerVisible(Killer, Victim))
							{
								if (Killer->IsAlive())
								{
									this->m_Player[KillerAuth].Stats[this->m_State].WallFrags++;
								}
							}
						}

						if (g_pGameRules)
						{
							auto NumAliveTR = 0, NumAliveCT = 0, NumDeadTR = 0, NumDeadCT = 0;

							CSGameRules()->InitializePlayerCounts(NumAliveTR, NumAliveCT, NumDeadTR, NumDeadCT);

							if (!NumDeadTR && !NumDeadCT)
							{
								this->m_Player[KillerAuth].Stats[this->m_State].EntryFrags++;

								this->m_Player[VictimAuth].Stats[this->m_State].EntryDeaths++;
							}

							if (NumAliveTR == 1 || NumAliveCT == 1)
							{
								for (int i = 1; i <= gpGlobals->maxClients; ++i)
								{
									auto Player = UTIL_PlayerByIndexSafe(i);

									if (Player)
									{
										if (Player->IsAlive())
										{
											auto Auth = this->GetAuthId(Player);

											if (Auth)
											{
												if ((Player->m_iTeam == TERRORIST) && (NumAliveTR == 1))
												{
													this->m_Player[Auth].Round.Versus = NumAliveCT;
												}
												else if ((Player->m_iTeam == CT) && (NumAliveCT == 1))
												{
													this->m_Player[Auth].Round.Versus = NumAliveTR;
												}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						this->m_Player[VictimAuth].Stats[this->m_State].Suicides++;
					}

					this->OnEvent(EVENT_PLAYER_DIED, ROUND_NONE, Victim, Killer);
				}
			}
		}
	}
}

void CPugStats::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (type == RT_ROUND_BONUS || type == RT_HOSTAGE_TOOK || type == RT_HOSTAGE_RESCUED || type == RT_ENEMY_KILLED || type == RT_VIP_KILLED || type == RT_VIP_RESCUED_MYSELF)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				this->m_Player[Auth].Stats[this->m_State].Money += amount;
			}
		}
	}
}

void CPugStats::RestartRound()
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (g_pGameRules)
		{
			// If is complete reset
			if (CSGameRules()->m_bCompleteReset)
			{
				// Loop all saved players
				for (auto& Player : this->m_Player)
				{
					// Reset player stats of this state
					Player.second.Stats[this->m_State].Reset();

					// Reset round stats
					Player.second.Round.Reset();
				}
			}
		}
	}
}

void CPugStats::RoundStart()
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		// For each player
		for (auto& Player : this->m_Player)
		{
			// Reset round stats
			Player.second.Round.Reset();
		}
	}
}

void CPugStats::RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (g_pGameRules)
		{
			if (winStatus == WINSTATUS_TERRORISTS || winStatus == WINSTATUS_CTS)
			{
				// Winner of Round
				auto Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

				// On Event Log
				this->OnEvent((Winner == TERRORIST) ? EVENT_TERRORISTS_WIN : EVENT_CTS_WIN, static_cast<int>(eventScenario), nullptr, nullptr);

				// Increment Score
				this->m_Match.Score[Winner]++;

				// Increment total rounds
				this->m_Match.Rounds++;

				// Calculate who is winning the match
				this->m_Match.Winner = (this->m_Match.Score[TERRORIST] != this->m_Match.Score[CT]) ? (this->m_Match.Score[TERRORIST] > this->m_Match.Score[CT] ? 1 : 2) : 0;

				// Loop player list
				for (auto& Player : this->m_Player)
				{
					// If is in winner team
					Player.second.Winner = (Player.second.Team == this->m_Match.Winner) ? 1 : 0;
				}

				// Team Round DAmage
				std::array<float, SPECTATOR + 1> TeamRoundDamage = { };

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
						{
							auto Auth = this->GetAuthId(Player);

							if (Auth)
							{
								if (this->m_Player[Auth].Round.Damage > 0)
								{
									TeamRoundDamage[Player->m_iTeam] += static_cast<float>(this->m_Player[Auth].Round.Damage);
								}
							}
						}
					}
				}

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					auto Player = UTIL_PlayerByIndexSafe(i);

					if (Player)
					{
						if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
						{
							auto Auth = this->GetAuthId(Player);

							if (Auth)
							{
								this->m_Player[Auth].Stats[this->m_State].RoundPlay++;

								if (Player->m_iTeam == Winner)
								{
									this->m_Player[Auth].Stats[this->m_State].RoundWin++;

									if (this->m_Player[Auth].Round.Versus > 0)
									{
										this->m_Player[Auth].Stats[this->m_State].Versus[this->m_Player[Auth].Round.Versus]++;
									}

									if (CSGameRules()->m_bBombDefused || CSGameRules()->m_bTargetBombed)
									{
										auto TotalPoints = gPugCvar.m_ST_RwsTotalPoints->value;

										if (this->m_Player[Auth].Round.BombDefused)
										{
											if (gPugCvar.m_ST_RwsC4Defused->value > 0.0f)
											{
												TotalPoints -= gPugCvar.m_ST_RwsC4Defused->value;

												this->m_Player[Auth].Stats[this->m_State].RoundWinShare += gPugCvar.m_ST_RwsC4Defused->value;
											}
										}

										if (this->m_Player[Auth].Round.BombExploded)
										{
											if (gPugCvar.m_ST_RwsC4Explode->value > 0.0f)
											{
												TotalPoints -= gPugCvar.m_ST_RwsC4Explode->value;

												this->m_Player[Auth].Stats[this->m_State].RoundWinShare += gPugCvar.m_ST_RwsC4Explode->value;
											}
										}

										if (this->m_Player[Auth].Round.Damage > 0)
										{
											this->m_Player[Auth].Stats[this->m_State].RoundWinShare += ((((float)this->m_Player[Auth].Round.Damage * (TotalPoints / 100.0f)) / TeamRoundDamage[Winner]) * 100.0f);
										}
									}
									else
									{
										if (this->m_Player[Auth].Round.Damage > 0)
										{
											this->m_Player[Auth].Stats[this->m_State].RoundWinShare += (((float)this->m_Player[Auth].Round.Damage / TeamRoundDamage[Winner]) * 100.0f);
										}
									}
								}
								else
								{
									this->m_Player[Auth].Stats[this->m_State].RoundLose++;
								}

								if (this->m_Player[Auth].Round.Frags > 0)
								{
									this->m_Player[Auth].Stats[this->m_State].KillStreak[this->m_Player[Auth].Round.Frags]++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CPugStats::MakeBomber(CBasePlayer* Player)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (Player->m_bHasC4)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				this->m_Player[Auth].Stats[this->m_State].BombSpawn++;
			}
		}
	}
}

void CPugStats::DropPlayerItem(CBasePlayer* Player, const char* pszItemName)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (pszItemName)
		{
			if (!Player->IsAlive())
			{
				if (Q_strcmp(pszItemName, "weapon_c4") == 0)
				{
					auto Auth = this->GetAuthId(Player);

					if (Auth)
					{
						this->m_Player[Auth].Stats[this->m_State].BombDrop++;
					}
				}
			}
		}
	}
}

void CPugStats::PlantBomb(entvars_t* pevOwner, bool Planted)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pevOwner));

		if (Player)
		{
			auto Auth = this->GetAuthId(Player);

			if (Auth)
			{
				if (Planted)
				{
					this->m_Player[Auth].Stats[this->m_State].BombPlanted++;

					this->OnEvent(EVENT_BOMB_PLANTED, ROUND_NONE, Player, nullptr);
				}
				else
				{
					this->m_Player[Auth].Stats[this->m_State].BombPlanting++;
				}
			}
		}
	}
}

void CPugStats::DefuseBombStart(CBasePlayer* Player)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		auto Auth = this->GetAuthId(Player);

		if (Auth)
		{
			this->m_Player[Auth].Stats[this->m_State].BombDefusing++;

			if (Player->m_bHasDefuser)
			{
				this->m_Player[Auth].Stats[this->m_State].BombDefusingKit++;
			}
		}
	}
}

void CPugStats::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		auto Auth = this->GetAuthId(Player);

		if (Auth)
		{
			if (Defused)
			{
				this->m_Player[Auth].Stats[this->m_State].BombDefused++;

				if (Player->m_bHasDefuser)
				{
					this->m_Player[Auth].Stats[this->m_State].BombDefusedKit++;
				}

				this->m_Player[Auth].Round.BombDefused = true;

				this->OnEvent(EVENT_BOMB_DEFUSED, ROUND_NONE, Player, nullptr);
			}
		}
	}
}

void CPugStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (this->m_State == STATE_FIRST_HALF || this->m_State == STATE_SECOND_HALF || this->m_State == STATE_OVERTIME)
	{
		if (pThis->m_bIsC4)
		{
			if (pThis->pev->owner)
			{
				auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pThis->pev->owner));

				if (Player)
				{
					auto Auth = this->GetAuthId(Player);

					if (Auth)
					{
						this->m_Player[Auth].Stats[this->m_State].BombExploded++;

						this->m_Player[Auth].Round.BombExploded = true;

						this->OnEvent(EVENT_BOMB_EXPLODED, ROUND_NONE, Player, nullptr);
					}
				}
			}
		}
	}
}

bool CPugStats::SayText(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	// If has entity target
	if (!FNullEnt(pEntity))
	{
		// Get CBasePlayer data
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		// If is not null
		if (Player)
		{
			// If is in game
			if ((Player->m_iTeam == TERRORIST) || (Player->m_iTeam == CT))
			{
				// Get argument 1
				auto Format = gPugEngine.GetString(1);

				// If is not empty
				if (Format)
				{
					// Get argument 3
					auto TextMsg = gPugEngine.GetString(3);

					// If is not empty
					if (TextMsg)
					{
						// If is chat for all or for all dead
						if (!Q_stricmp("#Cstrike_Chat_All", Format) || !Q_stricmp("#Cstrike_Chat_AllDead", Format))
						{
                            // If match is running
                            if (gPugStats.m_State != STATE_DEAD)
                            {
                                // If player is not null
                                if (Player)
                                {
                                    // If text is not null
                                    if (TextMsg)
                                    {
                                        // Get player auth
                                        auto Auth = gPugStats.GetAuthId(Player);

                                        // If is not null
                                        if (Auth)
                                        {
                                            // Chat struct
                                            P_PLAYER_CHAT Chat = { };

                                            // Set time
                                            Chat.Time = time(NULL);

                                            // Set match state
                                            Chat.State = gPugStats.m_State;

                                            // Set team
                                            Chat.Team = static_cast<int>(Player->m_iTeam);

                                            // Player is alive
                                            Chat.Alive = Player->IsAlive() ? 1 : 0;

                                            // Set message
                                            Chat.Message = TextMsg;

                                            // Push to vector
                                            gPugStats.m_Player[Auth].ChatLog.push_back(Chat);
                                        }
                                    }
                                }
                            }
						}
					}
				}
			}
		}
	}

	// Do not block original message call
	return false;
}

void CPugStats::OnEvent(GameEventType event, int ScenarioEvent, CBaseEntity* pEntity, class CBaseEntity* pEntityOther)
{
	// Round event data
	P_ROUND_EVENT Event = { };
	
	// Set event round count plus 1
	Event.Round = ((this->m_Match.Score[TERRORIST] + this->m_Match.Score[CT]) + 1);

	// If has ReGameDLL API Game Rules
	if (g_pGameRules)
	{
		// Get round timer
		Event.Time = CSGameRules()->GetRoundRemainingTimeReal();
	}
	
	// Store type of event
	Event.Type = event;
	
	// Store event scenario
	Event.ScenarioEvent = ScenarioEvent;
	
	// Switch of event
	switch (event)
	{
		// Tell bots the player is killed (argumens: 1 = victim, 2 = killer)
		case EVENT_PLAYER_DIED:
		{
			// Get Victim
			auto Victim = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// Get Killer
			auto Killer = UTIL_PlayerByIndexSafe(pEntityOther->entindex());

			// If event has victim abnd killer
			if (Victim && Killer)
			{
				// Get Killer auth index
				Event.Killer = this->GetAuthId(Killer);

				// Get Killer Origin
				Event.KillerOrigin = Killer->edict()->v.origin;

				// Get Victim auth index
				Event.Victim = this->GetAuthId(Victim);

				// Get Victim Origin
				Event.VictimOrigin = Victim->edict()->v.origin;

				// Get Winner of event (Team of Killer)
				Event.Winner = Killer->m_iTeam;

				// Get Loser of event (Team of Victim)
				Event.Loser = Victim->m_iTeam;

				// Check if victim was killed by a headshot
				Event.IsHeadShot = Victim->m_bHeadshotKilled ? 1 : 0;

				// Default weapon is empty
				Event.ItemIndex = WEAPON_NONE;

				// If killer has active item on hand
				if (Killer->m_pActiveItem)
				{
					// Store item index, and check if is HE Grenade
					Event.ItemIndex = (Victim->m_bKilledByGrenade ? WEAPON_HEGRENADE : Killer->m_pActiveItem->m_iId);
				}
			}

			//
			break;
		}
		// Tell bots the bomb has been planted (argumens: 1 = planter, 2 = NULL)
		case EVENT_BOMB_PLANTED:
		{
			// Get Bomb Planter
			auto Planter = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If found
			if (Planter)
			{
				// Store as killer auth index
				Event.Killer = this->GetAuthId(Planter);

				// Store killer origin
				Event.KillerOrigin = Planter->edict()->v.origin;
			}

			// Winner of that event is Terrorists
			Event.Winner = TERRORIST;

			// Loser of that event is CTs
			Event.Loser = CT;

			// Is not headshot, rofl
			Event.IsHeadShot = 0;

			// Weapon is C4
			Event.ItemIndex = WEAPON_C4;

			//
			break;
		}
		// Tell the bots the bomb is defused (argumens: 1 = defuser, 2 = NULL)
		case EVENT_BOMB_DEFUSED:
		{
			// Get Defuser of bomb
			auto Defuser = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If is not null
			if (Defuser)
			{
				// Set as killer
				Event.Killer = this->GetAuthId(Defuser);

				// Set origin
				Event.KillerOrigin = Defuser->edict()->v.origin;

				// If has defuser, set as headshot
				Event.IsHeadShot = Defuser->m_bHasDefuser ? 1 : 0;
			}

			// Winner is CT
			Event.Winner = CT;

			// Loser is TERRORIST
			Event.Loser = TERRORIST;

			// Defused item is C4
			Event.ItemIndex = WEAPON_C4;

			break;
		}
		// Let the bots hear the bomb exploding (argumens: 1 = NULL, 2 = NULL)
		case EVENT_BOMB_EXPLODED:
		{
			// Get Planter of the bomb
			auto Planter = UTIL_PlayerByIndexSafe(pEntity->entindex());

			// If found
			if (Planter)
			{
				// Set as killer
				Event.Killer = this->GetAuthId(Planter);

				// Set player origin
				Event.KillerOrigin = Planter->edict()->v.origin;
			}

			// Winner is TERRORIST
			Event.Winner = TERRORIST;

			// Loser is CT
			Event.Loser = CT;

			// Nothing to do here
			Event.IsHeadShot = 0;

			// Item index is C4
			Event.ItemIndex = WEAPON_C4;

			break;
		}
		// Tell bots the terrorists won the round (argumens: 1 = NULL, 2 = NULL)
		case EVENT_TERRORISTS_WIN:
		{
			// Set winner
			Event.Winner = TERRORIST;

			// Set loser
			Event.Loser = CT;

			// Nothing to do
			Event.IsHeadShot = false;

			// No weapons
			Event.ItemIndex = WEAPON_NONE;

			break;
		}
		// Tell bots the CTs won the round (argumens: 1 = NULL, 2 = NULL)
		case EVENT_CTS_WIN:
		{
			// Set winner
			Event.Winner = CT;

			// Set loser
			Event.Loser = TERRORIST;

			// Nothing to do
			Event.IsHeadShot = false;

			// No weapons
			Event.ItemIndex = WEAPON_NONE;

			break;
		}
		default:
		{
			// None
			break;
		}
	}

	// Insert Event Data
	this->m_RoundEvent.push_back(Event);
}

void CPugStats::ExportData()
{
	// Data
	nlohmann::ordered_json Data;

	// Match
	Data["Match"] =
	{
		{"StartTime", this->m_Match.StartTime},
		{"EndTime", this->m_Match.EndTime},
		{"HostName", this->m_Match.HostName},
		{"Map", this->m_Match.Map},
		{"Address", this->m_Match.Address},
		{"ScoreTRs", this->m_Match.Score[TERRORIST]},
		{"ScoreCTs", this->m_Match.Score[CT]},
		{"Winner", this->m_Match.Winner},
		{"Rounds", this->m_Match.Rounds},
		{"MaxRounds", this->m_Match.MaxRounds},
		{"MaxRoundsOT", this->m_Match.MaxRoundsOT},
		{"GameMode", this->m_Match.GameMode},
		{"KnifeRound", this->m_Match.KnifeRound},
	};

	// Player Loop
	for (auto const & Player : this->m_Player)
	{
		// Player Stats
		P_PLAYER_STATS PlayerStats = {};

		for (auto const & Stats : Player.second.Stats)
		{
			if (Stats.first == STATE_FIRST_HALF || Stats.first == STATE_SECOND_HALF || Stats.first == STATE_OVERTIME)
			{
				// Stats
				PlayerStats.Frags += Stats.second.Frags;
				PlayerStats.Deaths += Stats.second.Deaths;
				PlayerStats.Assists += Stats.second.Assists;
				PlayerStats.Headshots += Stats.second.Headshots;
				PlayerStats.Shots += Stats.second.Shots;
				PlayerStats.Hits += Stats.second.Hits;
				PlayerStats.HitsReceived += Stats.second.HitsReceived;
				PlayerStats.Damage += Stats.second.Damage;
				PlayerStats.DamageReceived += Stats.second.DamageReceived;
				PlayerStats.Money += Stats.second.Money;
				PlayerStats.Suicides += Stats.second.Suicides;
				//
				// Round Win Share
				PlayerStats.RoundWinShare += Stats.second.RoundWinShare;
				//
				// Sick Stats
				PlayerStats.BlindFrags += Stats.second.BlindFrags;
				PlayerStats.BlindDeaths += Stats.second.BlindDeaths;
				PlayerStats.OneShot += Stats.second.OneShot;
				PlayerStats.NoScope += Stats.second.NoScope;
				PlayerStats.FlyFrags += Stats.second.FlyFrags;
				PlayerStats.WallFrags += Stats.second.WallFrags;
				PlayerStats.GodLikes += Stats.second.GodLikes;
				PlayerStats.DoubleKill += Stats.second.DoubleKill;
				//
				// Knife Duels
				for (size_t i = 0; i < Stats.second.KnifeDuels.size(); i++)
				{
					PlayerStats.KnifeDuels[i] += Stats.second.KnifeDuels[i];
				}
				//
				// Entry Stats
				PlayerStats.EntryFrags += Stats.second.EntryFrags;
				PlayerStats.EntryDeaths += Stats.second.EntryDeaths;
				//
				// Rounds
				PlayerStats.RoundPlay += Stats.second.RoundPlay;
				PlayerStats.RoundWin += Stats.second.RoundWin;
				PlayerStats.RoundLose += Stats.second.RoundLose;
				//
				// Bomb
				PlayerStats.BombSpawn += Stats.second.BombSpawn;
				PlayerStats.BombDrop += Stats.second.BombDrop;
				PlayerStats.BombPlanting += Stats.second.BombPlanting;
				PlayerStats.BombPlanted += Stats.second.BombPlanted;
				PlayerStats.BombExploded += Stats.second.BombExploded;
				PlayerStats.BombDefusing += Stats.second.BombDefusing;
				PlayerStats.BombDefusingKit += Stats.second.BombDefusingKit;
				PlayerStats.BombDefused += Stats.second.BombDefused;
				PlayerStats.BombDefusedKit += Stats.second.BombDefusedKit;
				//
				// Kill streak
				for (size_t i = 0;i < Stats.second.KillStreak.size();i++)
				{
					PlayerStats.KillStreak[i] += Stats.second.KillStreak[i];
				}
				//
				// Versus
				for (size_t i = 0; i < Stats.second.Versus.size(); i++)
				{
					PlayerStats.Versus[i] += Stats.second.Versus[i];
				}
				//
				// HitBox (0 Hits, 1 Damage, 1 Hits Received, 3 Damage Received)
				for (size_t i = 0; i < Stats.second.HitBox.size(); i++)
				{
					for (size_t j = 0; j < Stats.second.HitBox[i].size(); j++)
					{
						PlayerStats.HitBox[i][j] += Stats.second.HitBox[i][j];
					}
				}
				//
				// Weapon Stats
				for (auto const& Weapon : Stats.second.Weapon)
				{
					PlayerStats.Weapon[Weapon.first].Frags += Weapon.second.Frags;
					PlayerStats.Weapon[Weapon.first].Deaths += Weapon.second.Deaths;
					PlayerStats.Weapon[Weapon.first].Headshots += Weapon.second.Headshots;
					PlayerStats.Weapon[Weapon.first].Shots += Weapon.second.Shots;
					PlayerStats.Weapon[Weapon.first].Hits += Weapon.second.Hits;
					PlayerStats.Weapon[Weapon.first].HitsReceived += Weapon.second.HitsReceived;
					PlayerStats.Weapon[Weapon.first].Damage += Weapon.second.Damage;
					PlayerStats.Weapon[Weapon.first].DamageReceived += Weapon.second.DamageReceived;
				}
			}
		}
		//
		// Json Stats
		Data["Stats"][Player.first] =
		{
			// Player data
			{"JoinGameTime",Player.second.JoinGameTime},
			{"DisconnectTime",Player.second.DisconnectTime},
			{"Name",Player.second.Name},
			{"Team",Player.second.Team},
			{"Winner",Player.second.Winner},
			{"IsBot",Player.second.IsBot},
			//
			// Player stats
			{"Frags",PlayerStats.Frags},
			{"Deaths",PlayerStats.Deaths},
			{"Assists",PlayerStats.Assists},
			{"Headshots",PlayerStats.Headshots},
			{"Shots",PlayerStats.Shots},
			{"Hits",PlayerStats.Hits},
			{"HitsReceived",PlayerStats.HitsReceived},
			{"Damage",PlayerStats.Damage},
			{"DamageReceived",PlayerStats.DamageReceived},
			{"Money",PlayerStats.Money},
			{"Suicides",PlayerStats.Suicides},
			//
			// Round Win Share
			{"RoundWinShare",(PlayerStats.RoundWinShare / static_cast<float>(this->m_Match.Rounds))},
			//
			// Misc Frags
			{"BlindFrags",PlayerStats.BlindFrags},
			{"BlindDeaths",PlayerStats.BlindDeaths},
			{"OneShot",PlayerStats.OneShot},
			{"NoScope",PlayerStats.NoScope},
			{"FlyFrags",PlayerStats.FlyFrags},
			{"WallFrags",PlayerStats.WallFrags},
			{"GodLikes",PlayerStats.GodLikes},
			{"DoubleKill",PlayerStats.DoubleKill},
			//
			// Knife Duels
			{"KnifeDuelWin",PlayerStats.KnifeDuels[0]},
			{"KnifeDuelLose",PlayerStats.KnifeDuels[1]},
			//
			// Entry Frags and Deaths
			{"EntryFrags",PlayerStats.EntryFrags},
			{"EntryDeaths",PlayerStats.EntryDeaths},
			//
			// Round counter
			{"RoundPlay",PlayerStats.RoundPlay},
			{"RoundWin",PlayerStats.RoundWin},
			{"RoundLose",PlayerStats.RoundLose},
			//
			// Bomb counter
			{"BombSpawn",PlayerStats.BombSpawn},
			{"BombDrop",PlayerStats.BombDrop},
			{"BombPlanting",PlayerStats.BombPlanting},
			{"BombPlanted",PlayerStats.BombPlanted},
			{"BombExploded",PlayerStats.BombExploded},
			{"BombDefusing",PlayerStats.BombDefusing},
			{"BombDefusingKit",PlayerStats.BombDefusingKit},
			{"BombDefused",PlayerStats.BombDefused},
			{"BombDefusedKit",PlayerStats.BombDefusedKit},
			//
			// Kill Streak
			{"KillStreak",PlayerStats.KillStreak},
			//
			// Versus
			{"Versus",PlayerStats.Versus},
			//
			// Hitbox Data
			{"HitBox",PlayerStats.HitBox}
		};
		//
		// Weapons
		for (auto const& Weapon : PlayerStats.Weapon)
		{
			Data["Stats"][Player.first]["Weapon"][std::to_string(Weapon.first)] =
			{
				{"Frags", Weapon.second.Frags},
				{"Deaths", Weapon.second.Deaths},
				{"Headshots", Weapon.second.Headshots},
				{"Shots", Weapon.second.Shots},
				{"Hits", Weapon.second.Hits},
				{"HitsReceived", Weapon.second.HitsReceived},
				{"Damage", Weapon.second.Damage},
				{"DamageReceived", Weapon.second.DamageReceived}
			};
		}
		//
		// Chat log
		for (auto const& Chat : Player.second.ChatLog)
		{
			Data["Stats"][Player.first]["Chat"].push_back
			({
				{"Time", Chat.Time},
				{"State", Chat.State},
				{"Team", Chat.Team},
				{"Alive", Chat.Alive},
				{"Message", Chat.Message}
			});
		}
	}

	// Round events
	for (auto const& Event : this->m_RoundEvent)
	{
		Data["Events"].push_back
		({
			{"Round",Event.Round},
			{"Time",Event.Time},
			{"Type",Event.Type},
			{"ScenarioEvent", Event.ScenarioEvent},
			{"Winner",Event.Winner},
			{"Loser",Event.Loser},
			{"Killer",Event.Killer.c_str()},
			{"KillerOrigin",{Event.KillerOrigin.x,Event.KillerOrigin.y,Event.KillerOrigin.z}},
			{"Victim",Event.Victim.c_str()},
			{"VictimOrigin",{Event.VictimOrigin.x,Event.VictimOrigin.y,Event.VictimOrigin.z}},
			{"IsHeadShot",Event.IsHeadShot},
			{"ItemIndex",Event.ItemIndex}
		});
	}

	if (!Data.empty())
	{
		char DateTime[32] = {};
		strftime(DateTime, sizeof(DateTime), "%y%m%d%H%M", localtime(&this->m_Match.EndTime));

		auto FullPath = gPugUtil.GetFullPath();

		char Buffer[MAX_PATH] = {};
		Q_snprintf(Buffer, sizeof(Buffer), "%s/stats", FullPath);

		gPugUtil.MakeDirectory(Buffer);

		Q_snprintf(Buffer, sizeof(Buffer), "%s/stats/pug-%s-%s.json", FullPath, DateTime, this->m_Match.Map.c_str());

		std::ofstream DataFile(Buffer);

		DataFile << Data;

		DataFile.close();

		if (gPugCvar.m_API_Enable->value)
		{
			if (gPugCvar.m_API_Address->string)
			{
                if (gPugCvar.m_API_Address->string[0u] != '\0')
                {
                    gPugCurl.PostJSON(gPugCvar.m_API_Address->string, static_cast<long>(gPugCvar.m_API_Timeout->value), gPugCvar.m_API_Bearer->string, Data.dump());
                }
			}
		}
	}

	Data.clear();
}
