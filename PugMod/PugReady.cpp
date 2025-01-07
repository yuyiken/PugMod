#include "precompiled.h"

CPugReady gPugReady;

void CPugReady::ServerActivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Ready.fill(false);
}

void CPugReady::ServerDeactivate()
{
    this->m_Run = false;

    this->m_NextFrame = 0.0f;

    this->m_Ready.fill(false);
}

void CPugReady::Init()
{
    this->m_Run = true;

    this->m_NextFrame = 0.0f;

    this->m_Ready.fill(false);

    gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 A partida vai começar quando todos estiverem prontos.", gPugCvar.m_Tag->string);
}

void CPugReady::Stop()
{
    if (this->m_Run)
    {
        this->m_Run = 0;

        this->m_NextFrame = 0.0f;

        this->m_Ready.fill(false);

        gPugMod.NextState();
    }
}

void CPugReady::PutInServer(edict_t *pEntity)
{
    if (!FNullEnt(pEntity))
    {
        this->m_Ready[ENTINDEX(pEntity)] = false;
    }
}

void CPugReady::GetIntoGame(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        this->m_Ready[Player->entindex()] = false;
    }
}

void CPugReady::PlayerSpawn(CBasePlayer *Player)
{
    if (this->m_Run)
    {
        if (!this->m_Ready[Player->entindex()])
        {
            auto pCmd = gPugClientCmd.Get(CMD_READY);

            if (pCmd)
            {
                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Quando você estiver pronto, digite ^3%s^1.", gPugCvar.m_Tag->string, pCmd->Name.c_str());
            }
        }
    }
}

void CPugReady::Disconnect(edict_t *pEntity)
{
    if (!FNullEnt(pEntity))
    {
        this->m_Ready[ENTINDEX(pEntity)] = false;
    }
}

void CPugReady::RestartRound()
{
    if (this->m_Run)
    {
        if (g_pGameRules)
        {
            if (CSGameRules()->m_bCompleteReset)
            {
                this->Init();
            }
        }
    }
}

void CPugReady::StartFrame()
{
    if (this->m_Run)
    {
        if (gpGlobals->time >= this->m_NextFrame)
        {
            std::string PlayerList[2];

            std::array<short, 2> ReadyCount;
            
            for (int i = 1; i <= gpGlobals->maxClients; ++i)
            {
                auto Player = UTIL_PlayerByIndex(i);

                if (Player)
                {
                    if (!Player->IsDormant())
                    {
                        if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                        {
                            auto IsReady = (Player->IsBot() || this->m_Ready[i]) ? 1 : 0;

                            ReadyCount[IsReady]++;

                            PlayerList[IsReady].append(STRING(Player->edict()->v.netname));

                            PlayerList[IsReady].append("\n");
                        }
                    }
                }
            }

            if (ReadyCount[1] >= static_cast<int>(gPugCvar.m_PlayersMin->value))
            {
                gPugUtil.PrintColor(nullptr, E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Todos os jogadores estão prontos!", gPugCvar.m_Tag->string);

                this->Stop();
            }
            else
            {
                if (!gPugCvar.m_ReadyList->value)
                {
                    gPugUtil.SendHud(nullptr, g_ReadyList_HudParam[0], "Aquecendo (%d de %d):", ReadyCount[0], static_cast<int>(gPugCvar.m_PlayersMin->value));
                    gPugUtil.SendHud(nullptr, g_ReadyList_HudParam[1], "Pronto (%d de %d):", ReadyCount[1], static_cast<int>(gPugCvar.m_PlayersMin->value));

                    if (!PlayerList[0].empty())
                    {
                        gPugUtil.SendHud(nullptr, g_ReadyList_HudParam[2], "\n%s", PlayerList[0].c_str());
                    }
                    
                    if (!PlayerList[1].empty())
                    {
                        gPugUtil.SendHud(nullptr, g_ReadyList_HudParam[3], "\n%s", PlayerList[1].c_str());
                    }
                }
                else
                {
                    gPugUtil.SendHud
                    (
                        nullptr,
                        g_ReadyNum_HudParam,
                        "Aquecendo: %d / %d\nPronto: %d / %d",
                        ReadyCount[0],
                        static_cast<int>(gPugCvar.m_PlayersMin->value),
                        ReadyCount[1],
                        static_cast<int>(gPugCvar.m_PlayersMin->value)
                    );
                }
            }

            this->m_NextFrame = (gpGlobals->time + 0.9f);
        }
    }
}

bool CPugReady::Ready(CBasePlayer *Player)
{
	if (this->m_Run)
	{
		if (!this->m_Ready[Player->entindex()])
		{
			if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
			{
				this->m_Ready[Player->entindex()] = true;

				gPugUtil.PrintColor(nullptr, Player->entindex(), "^4[%s]^1 ^3%s^1 está pronto.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname));

				return true;
			}
		}
        else
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você já está pronto.", gPugCvar.m_Tag->string);
        }

        return false;
	}

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);

	return false;
}

bool CPugReady::NotReady(CBasePlayer *Player)
{
	if (this->m_Run)
	{
		if (this->m_Ready[Player->entindex()])
		{
			if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
			{
				this->m_Ready[Player->entindex()] = false;

				gPugUtil.PrintColor(nullptr, Player->entindex(), "^4[%s]^1 ^3%s^1 não está pronto.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname));

				return true;
			}
		}
        else
        {
            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você ainda não está pronto.", gPugCvar.m_Tag->string);
        }
        return false;
	}

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Comando indisponível.", gPugCvar.m_Tag->string);

	return false;
}