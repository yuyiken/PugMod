#include "precompiled.h"

CPugCaptain gPugCaptain;

void CPugCaptain::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_PlayersMin = 0;

	this->m_Team.clear();
}

void CPugCaptain::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_PlayersMin = 0;

	this->m_Team.clear();
}

void CPugCaptain::Init()
{
	auto Players = gPugUtil.GetPlayers(false, true);

    this->m_PlayersMin = static_cast<int>(gPugCvar.m_PlayersMin->value / 2.0f);

	if (Players.size() >= this->m_PlayersMin)
	{
		this->m_Run = true;

        this->m_NextFrame = 0.0f;

		this->m_Team.clear();

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AutoTeamJoin, "1");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_HumansJoinTeam, "SPEC");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotJoinTeam, "SPEC");

		g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AllowSpectators, "1");

		for (auto & Player : Players)
		{
			if (Player->m_iTeam)
			{
				this->m_Team[Player->entindex()] = Player->m_iTeam;

				Player->edict()->v.deadflag = DEAD_DEAD;

				Player->CSPlayer()->JoinTeam(SPECTATOR);

				Player->ClearConditions(BIT_IS_LEADER);

				Player->ClearConditions(BIT_IS_INMENU);
			}
		}

		auto Team = static_cast<TeamName>(g_engfuncs.pfnRandomLong(1, 2));

		this->SetCaptain(Players.at(0), Team);

		this->SetCaptain(Players.at(1), (Team == TERRORIST) ? CT : TERRORIST);

		this->Menu(Players.at(g_engfuncs.pfnRandomLong(0, 1)));
	}
	else
	{
		this->Stop();
	}
}

void CPugCaptain::Stop()
{
	this->m_Run = false;

    this->m_NextFrame = 0.0f;

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AutoTeamJoin, "0");

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_HumansJoinTeam, "any");

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotJoinTeam, "any");

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_AllowSpectators, "0");

	auto Players = gPugUtil.GetPlayers();

	if ((Players[TERRORIST].size() >= this->m_PlayersMin) && (Players[CT].size() >= this->m_PlayersMin))
	{
		if (gPugCvar.m_KnifeRound->value)
		{
			gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_KNIFE_ROUND);
		}
		else
		{
			gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_FIRST_HALF);
		}

        gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Os times estão completos.", gPugCvar.m_Tag->string);
	}
	else
	{
		if (!this->m_Team.empty())
		{
			for (auto const & Backup : this->m_Team)
			{
				auto Player = UTIL_PlayerByIndexSafe(Backup.first);

				if (Player)
				{
					if (Player->m_iTeam != Backup.second)
					{
						Player->edict()->v.deadflag = DEAD_DEAD;
						
						Player->CSPlayer()->JoinTeam(Backup.second);

						Player->edict()->v.deadflag = DEAD_NO;
					}
				}
			}
		}

		gPugTask.Create(E_TASK::SET_STATE, 2.0f, false, STATE_DEATHMATCH);

		gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Falha ao escolher: ^3Os times estão incompletos.^1", gPugCvar.m_Tag->string);
	}

	
}

void CPugCaptain::DropClient(edict_t *pEntity)
{
	auto Captain = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

	if (Captain->HasConditions(BIT_IS_LEADER))
	{
		auto Players = gPugUtil.GetPlayers(SPECTATOR);

		if (Players.size())
		{
			this->SetCaptain(Players.at(0), Captain->m_iTeam);

			if (Captain->HasConditions(BIT_IS_INMENU))
			{
				this->Menu(Players.at(0));
			}
		}
		else
		{
			this->Stop();
		}
	}
}

void CPugCaptain::SetCaptain(CBasePlayer* Player, TeamName Team)
{
    if (Player)
    {
        Player->CSPlayer()->JoinTeam(Team);

        if (!Player->IsAlive())
        {
            Player->RoundRespawn();
        }

        Player->SetConditions(BIT_IS_LEADER);

        Player->ClearConditions(BIT_IS_INMENU);

        gPugUtil.PrintColor(nullptr, (Team == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), "^4[%s]^1 Novo capitão ^3%s^1: %s", gPugCvar.m_Tag->string, (Team == TERRORIST ? "Terrorista" : "CT"), STRING(Player->edict()->v.netname));
    }
}

void CPugCaptain::GetPlayer(CBasePlayer* Captain, CBasePlayer* Target)
{
	if (Captain)
	{
		gPugTask.Remove(E_TASK::CAPTAIN_MENU);

		gPugMenu[Captain->entindex()].Hide(Captain);

		if (Target)
		{
			if (Target->m_iTeam == SPECTATOR)
			{
				Target->CSPlayer()->JoinTeam(Captain->m_iTeam);

				if (!Target->IsAlive())
				{
					Target->RoundRespawn();
				}

				Captain->ClearConditions(BIT_IS_INMENU);

				gPugUtil.PrintColor(nullptr, (Captain->m_iTeam == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), "^4[%s]^1 ^3%s^1 escolheu ^3%s^1", gPugCvar.m_Tag->string, STRING(Captain->edict()->v.netname), STRING(Target->edict()->v.netname));

				this->NextMenu(Captain);
			}
			else
			{
				gPugUtil.PrintColor(nullptr, (Target->m_iTeam == TERRORIST ? E_PRINT_TEAM::RED : E_PRINT_TEAM::BLUE), "^4[%s]^1 ^3%s^1 já está em um time.", gPugCvar.m_Tag->string, STRING(Target->edict()->v.netname));

				this->Menu(Captain);
			}
		}
		else
		{
			auto Players = gPugUtil.GetPlayers(SPECTATOR);

			if (Players.size())
			{
				this->GetPlayer(Captain, Players.at(0));
			}
		}
	}
}

void CPugCaptain::NextMenu(CBasePlayer* Captain)
{
	auto Players = gPugUtil.GetPlayers();

	if (Players[SPECTATOR].size() > 0)
	{
		for (auto const& Player : Players[Captain->m_iTeam == TERRORIST ? CT : TERRORIST])
		{
			if (Player->HasConditions(BIT_IS_LEADER))
			{
				if (Player->entindex() != Captain->entindex())
				{
					if (Players[Player->m_iTeam].size() < this->m_PlayersMin)
					{
						this->Menu(Player);
						return;
					}
				}
			}
		}

		if (Players[Captain->m_iTeam].size() < this->m_PlayersMin)
		{
			this->Menu(Captain);
			return;
		}
	}
    else
    {
        this->Stop();
    }
}

void CPugCaptain::Menu(CBasePlayer* Captain)
{
    if (Captain)
    {
        auto Players = gPugUtil.GetPlayers(SPECTATOR);

        if (Players.size() == 1 || Captain->IsBot())
        {
			gPugTask.Create(E_TASK::CAPTAIN_MENU, 2.0f, false, Captain->entindex());
        }
        else
        {
            gPugMenu[Captain->entindex()].Create("Jogadores:", false, E_MENU::CP_CAPTAIN_MENU);

            for (auto const& Player : Players)
            {
                gPugMenu[Captain->entindex()].AddItem(Player->entindex(), STRING(Player->edict()->v.netname), false, Player->m_iTeam);
            }

            Captain->SetConditions(BIT_IS_INMENU);

            gPugMenu[Captain->entindex()].Show(Captain);

            gPugTask.Create(E_TASK::CAPTAIN_MENU, 10.0f, false, Captain->entindex());

			gPugUtil.PrintColor(Captain->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Escolha um jogador em 10 segundos.", gPugCvar.m_Tag->string);
        }
    }
}

void CPugCaptain::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		auto Spectactor = UTIL_PlayerByIndexSafe(Item.Info);

		if (Spectactor)
		{
			gPugCaptain.GetPlayer(Player, Spectactor);
		}
	}
}

void CPugCaptain::GetRandomPlayer(int EntityIndex)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gPugCaptain.GetPlayer(Player, nullptr);
	}
}

void CPugCaptain::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            std::map<int, std::string> PlayerList = {};

            std::map<int, int> PlayerCount = {};

            for (auto i = 1; i <= gpGlobals->maxClients; ++i)
            {
                auto Player = UTIL_PlayerByIndexSafe(i);

                if (Player)
                {
                    if (!Player->IsDormant())
                    {
                        if (Player->m_iTeam != UNASSIGNED)
                        {
                            PlayerCount[Player->m_iTeam]++;

                            PlayerList[Player->m_iTeam] += STRING(Player->edict()->v.netname);

                            if (Player->HasConditions(BIT_IS_LEADER))
                            {
                                if (Player->HasConditions(BIT_IS_INMENU))
                                {
                                    PlayerList[Player->m_iTeam] += " (C) *";
                                }
                                else
                                {
                                    PlayerList[Player->m_iTeam] += " (C)";
                                }
                            }

                            PlayerList[Player->m_iTeam] += "\n";
                        }
                    }
                }
            }

            for (auto i = 0; i < 5 - PlayerCount[CT]; i++)
            {
                PlayerList[CT] += "\n";
            }

            gPugUtil.SendHud(nullptr, g_Captain_HudParam[0], "Terroristas (%d / %d):", PlayerCount[TERRORIST], this->m_PlayersMin);

            gPugUtil.SendHud(nullptr, g_Captain_HudParam[1], "\n%s", PlayerList[TERRORIST].c_str());

            if (PlayerCount[SPECTATOR] < 1)
            {
                gPugUtil.SendHud(nullptr, g_Captain_HudParam[2], "CTs (%d / %d):", PlayerCount[CT], this->m_PlayersMin);

                gPugUtil.SendHud(nullptr, g_Captain_HudParam[3], "\n%s", PlayerList[CT].c_str());
            }
            else
            {
                gPugUtil.SendHud(nullptr, g_Captain_HudParam[4], "CTs (%d / %d):\n\n\n\n\n\nEspectadores (%d):", PlayerCount[CT], this->m_PlayersMin, PlayerCount[SPECTATOR]);

                gPugUtil.SendHud(nullptr, g_Captain_HudParam[5], "\n%s\n%s", PlayerList[CT].c_str(), PlayerList[SPECTATOR].c_str());
            }

            this->m_NextFrame = (gpGlobals->time + 0.9f);
        }
    }
}
