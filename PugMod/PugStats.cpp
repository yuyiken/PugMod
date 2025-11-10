#include "precompiled.h"

CPugStats gPugStats;

void CPugStats::RoundStart()
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		this->m_RoundDmg = {};
		this->m_RoundHit = {};

        this->m_Flags = R_STATS_ALL;

        if (gPugCvar.m_RoundStats)
        {
            if (gPugCvar.m_RoundStats->string)
            {
                if (gPugCvar.m_RoundStats->string[0u] != '\0')
                {
                    this->m_Flags |= gPugAdmin.ReadFlags(gPugCvar.m_RoundStats->string);
                }
            }
        }
	}
}

void CPugStats::RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay)
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
		if (winStatus != WINSTATUS_NONE)
		{
			auto Players = gPugUtil.GetPlayers(true, false);

            if (Players.size() > 0)
            {
                for (auto const& Player : Players)
                {
                    if (Player->IsAlive())
                    {
                        this->ShowStats(Player);
                    }
                }
            }
		}
	}
}

void CPugStats::GetIntoGame(CBasePlayer *Player)
{
    this->m_RoundDmg[Player->entindex()] = {};
    this->m_RoundHit[Player->entindex()] = {};
}

void CPugStats::DropClient(edict_t *pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto EntityIndex = ENTINDEX(pEntity);

		if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
		{
			for (auto i = 0; i <= gpGlobals->maxClients; i++)
			{
				this->m_RoundDmg[i][EntityIndex] = 0;
				this->m_RoundHit[i][EntityIndex] = 0;
			}
		}
	}
}

void CPugStats::TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType)
{
    auto State = gPugMod.GetState();

	if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
	{
        auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

		if (!Player->m_bKilledByBomb)
		{
			if (Attacker)
			{
				if (Player->entindex() != Attacker->entindex())
				{
					if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker))
					{
						this->m_RoundDmg[Attacker->entindex()][Player->entindex()] += flDamage;
						this->m_RoundHit[Attacker->entindex()][Player->entindex()] += 1;
					}
				}
			}
		}
		else
		{
            gPugUtil.SendDeathMsg(NULL, NULL, Player, NULL, NULL, "c4", 0, 0);
		}
	}
}

void CPugStats::SendDeathMessage(CBaseEntity *KillerBaseEntity, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
    auto State = gPugMod.GetState();

    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        this->ShowStats(Victim);
    }
}

bool CPugStats::ShowHP(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();

    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_HP)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating)
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (Target->m_iTeam != Player->m_iTeam)
                            {
                                if (Target->IsAlive())
                                {
                                    StatsCount++;

                                    gPugUtil.PrintColor(Player->edict(), Target->entindex(), _T("^4[%s]^1 ^3%s^1 com %d HP (%d AP)"), gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname), (int)Target->edict()->v.health, (int)Target->edict()->v.armorvalue);
                                }
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Nenhum jogador vivo."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Comando indisponível."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugStats::ShowDamage(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_DMG)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Player->entindex()][Target->entindex()])
                            {
                                StatsCount++;

                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 Acertou ^3%s^1 %d vez(es) (Dano %d)"),
                                    gPugCvar.m_Tag->string,
                                    STRING(Target->edict()->v.netname),
                                    this->m_RoundHit[Player->entindex()][Target->entindex()],
                                    this->m_RoundDmg[Player->entindex()][Target->entindex()]
                                );
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Nenhum dano nesse round."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Comando indisponível."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugStats::ShowReceivedDamage(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_RDMG)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;

                        auto Players = gPugUtil.GetPlayers(true, true);

                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Target->entindex()][Player->entindex()])
                            {
                                StatsCount++;

                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 Dano Recebido de ^3%s^1 %d vez(es) (Dano %d)"),
                                    gPugCvar.m_Tag->string,
                                    STRING(Target->edict()->v.netname),
                                    this->m_RoundHit[Target->entindex()][Player->entindex()],
                                    this->m_RoundDmg[Target->entindex()][Player->entindex()]
                                );
                            }
                        }

                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Nenhum dano recebido nesse round."), gPugCvar.m_Tag->string);
                        }

                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Comando indisponível."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugStats::ShowSummary(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_SUM)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        auto StatsCount = 0;
    
                        auto Players = gPugUtil.GetPlayers(true, true);
    
                        for (auto Target : Players)
                        {
                            if (this->m_RoundHit[Player->entindex()][Target->entindex()] || this->m_RoundHit[Target->entindex()][Player->entindex()])
                            {
                                StatsCount++;
    
                                gPugUtil.PrintColor
                                (
                                    Player->edict(),
                                    Target->entindex(),
                                    _T("^4[%s]^1 (%d dano | %d hits) para (%d dano | %d hits) de ^3%s^1 (%d HP)"),
                                    gPugCvar.m_Tag->string,
                                    this->m_RoundDmg[Player->entindex()][Target->entindex()],
                                    this->m_RoundHit[Player->entindex()][Target->entindex()],
                                    this->m_RoundDmg[Target->entindex()][Player->entindex()],
                                    this->m_RoundHit[Target->entindex()][Player->entindex()],
                                    STRING(Target->edict()->v.netname),
                                    Target->IsAlive() ? (int)Target->edict()->v.health : 0
                                );
                            }
                        }
    
                        if (!StatsCount)
                        {
                            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Nenhum dano nesse round."), gPugCvar.m_Tag->string);
                        }
    
                        return true;
                    }
                }
            }
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Comando indisponível."), gPugCvar.m_Tag->string);

	return false;
}

bool CPugStats::ShowStats(CBasePlayer *Player)
{
    auto State = gPugMod.GetState();
        
    if (State == STATE_FIRST_HALF || State == STATE_SECOND_HALF || State == STATE_OVERTIME)
    {
        if (this->m_Flags & R_STATS_STATS)
        {
            if (g_pGameRules)
            {
                if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
                {
                    if (!Player->IsBot())
                    {
                        if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                        {
                            auto Players = gPugUtil.GetPlayers(true, true);
    
                            if (Players.size() > 0)
                            {
                                char HudList[3][511] = {};
    
                                Q_memset(HudList, 0, sizeof(HudList));
    
                                auto PlayerId = Player->entindex();
    
                                for (auto const& Target : Players)
                                {
                                    auto TargetId = Target->entindex();
    
                                    if (this->m_RoundHit[PlayerId][TargetId])
                                    {
                                        Q_snprintf(HudList[0] + strlen(HudList[0]), sizeof(HudList[0]), _T("%s -- %d hit(s) / %d dmg\n"), STRING(Target->edict()->v.netname), this->m_RoundHit[PlayerId][TargetId], this->m_RoundDmg[PlayerId][TargetId]);
                                    }
    
                                    if (this->m_RoundHit[TargetId][PlayerId])
                                    {
                                        Q_snprintf(HudList[1] + strlen(HudList[1]), sizeof(HudList[1]), _T("%s -- %d hit(s) / %d dmg\n"), STRING(Target->edict()->v.netname), this->m_RoundHit[TargetId][PlayerId], this->m_RoundDmg[TargetId][PlayerId]);
                                    }
                                }
    
                                if (HudList[0][0u] != '\0')
                                {
                                    Q_snprintf(HudList[2], sizeof(HudList[2]), _T("Victims:^n%s"), HudList[0]);
                                }
    
                                if (HudList[1][0u] != '\0')
                                {
                                    Q_snprintf(HudList[2] + strlen(HudList[2]), sizeof(HudList[2]), _T("^n^n^n^nAttackers:\n%s"), HudList[1]);
                                }
    
                                if (HudList[2][0u] != '\0')
                                {
                                    gPugUtil.SendHud(Player->edict(), g_RoundStats_HudParam, HudList[2]);
                                }
                            }
    
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}