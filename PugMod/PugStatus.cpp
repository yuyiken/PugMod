#include "precompiled.h"

CPugStatus gPugStatus;

void CPugStatus::ServerActivate()
{
    this->m_NextUpdate = (gpGlobals->time + gPugCvar.m_API_Status->value);

    this->m_Data.clear();
}

void CPugStatus::StartFrame()
{
    if (gPugCvar.m_API_Enable->value > 0.0f)
    {
        if (gPugCvar.m_API_Address->string)
        {
            if (gPugCvar.m_API_Address->string[0u] != '\0')
            {
                if (gPugCvar.m_API_Status->value >= PUG_STATUS_MIN_UPDATE_TIME)
                {
                    if (gpGlobals->time >= this->m_NextUpdate)
                    {
                        this->SendStatus();

                        this->m_NextUpdate = (gpGlobals->time + gPugCvar.m_API_Status->value);
                    }
                }
            }
        }
    }
}

void CPugStatus::SendStatus()
{
	this->m_Data["Time"] = time(NULL);

    this->m_Data["Address"] = g_engfuncs.pfnCVarGetString("net_address");

    this->m_Data["Hostname"] = g_engfuncs.pfnCVarGetString("hostname");

    this->m_Data["Map"] = STRING(gpGlobals->mapname);

    this->m_Data["Game"] = (g_pGameRules ? CSGameRules()->m_GameDesc : "Counter-Strike");

    this->m_Data["KnifeRound"] = static_cast<int>(gPugCvar.m_PlayKnifeRound->value);

    this->m_Data["GameMode"] = static_cast<int>(gPugCvar.m_PlayGameMode->value);

    this->m_Data["State"] = gPugMod.GetString(gPugMod.GetState());

    this->m_Data["ScoreTR"] = gPugMod.GetScore(TERRORIST);

    this->m_Data["ScoreCT"] = gPugMod.GetScore(CT);

    this->m_Data["Rounds"] = gPugMod.GetRound();

    this->m_Data["MaxRounds"] = static_cast<int>(gPugCvar.m_Rounds->value);

    this->m_Data["NumPlayers"] = 0;

    this->m_Data["MaxPlayers"] = gpGlobals->maxClients;

    auto Players = gPugUtil.GetPlayers(false, true);

    if (Players.size() > 0)
    {
        this->m_Data["NumPlayers"] = static_cast<int>(Players.size());

        for (auto const & Player : Players)
        {
            auto Auth = gPugStats.GetAuthId(Player);

            if (Auth)
            {
                if (Auth[0u] != '\0')
                {
                    this->m_Data["Players"][std::to_string(Player->entindex())] =
                    {
                        {"EntityId", Player->entindex()},
                        {"Auth", Auth},
                        {"Name", STRING(Player->edict()->v.netname)},
                        {"UserId", g_engfuncs.pfnGetPlayerUserId(Player->edict())},
                        {"Team", Player->m_iTeam},
                        {"Frags", static_cast<int>(Player->edict()->v.frags)},
                        {"Deaths", Player->m_iDeaths},
                    };
                }
            }
        }   
    }

    if (!this->m_Data.empty())
    {
        gPugCurl.PostJSON(gPugCvar.m_API_Address->string, (long)(gPugCvar.m_API_Timeout->value), gPugCvar.m_API_Bearer->string, this->m_Data.dump());
    }
}
