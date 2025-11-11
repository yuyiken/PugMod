#include "precompiled.h"

CPugRetry gPugRetry;

void CPugRetry::PutInServer(edict_t *pEntity)
{   
    if (!FNullEnt(pEntity))
    {
        if (gPugCvar.m_RetryTime->value > 0.0f)
        {
            auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEntity);

            if (Auth)
            {
                if (Auth[0u] != '\0')
                {
                    if (gPugUtil.IsNetClient(pEntity))
                    {
                        if (this->m_Data.find(Auth) != this->m_Data.end())
                        {
                            auto TimeLeft = (this->m_Data[Auth] - gpGlobals->time);

                            if (TimeLeft > 0)
                            {
                                gPugUtil.ClientDrop(ENTINDEX(pEntity), _T("Wait %d more second(s) before reconnecting."), (int)(TimeLeft));
                            }
                            else
                            {
                                this->m_Data.erase(Auth);
                            }
                        }
                    }
                }
            }
        }
    }
}

void CPugRetry::DropClient(edict_t *pEntity, bool crash, const char *Reason)
{
    if (!FNullEnt(pEntity))
    {
        if (gPugCvar.m_RetryTime->value > 0.0f)
        {
            auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

            if (Player)
            {
                if (!gPugAdmin.Access(Player->entindex(), ADMIN_IMMUNITY))
                {
                    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                    {
                        if (!Player->IsBot())
                        {
                            if (gPugUtil.IsNetClient(pEntity))
                            {
                                auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEntity);

                                if (Auth)
                                {
                                    if (Auth[0u] != '\0')
                                    {
                                        if (Q_stricmp(Reason, "Client sent 'drop'") || Q_stricmp(Reason, "Timed out"))
                                        {
                                            this->m_Data[Auth] = (gpGlobals->time + gPugCvar.m_RetryTime->value);
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
}