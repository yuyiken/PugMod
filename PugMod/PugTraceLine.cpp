#include "precompiled.h"

CPugTraceLine gPugTraceLine;

void CPugTraceLine::TraceLine(const float* vStart, const float* vEnd, int fNoMonsters, edict_t* pentToSkip, TraceResult* pTraceResult)
{
	if ((fNoMonsters == dont_ignore_monsters) && (gpGlobals->trace_flags != FTRACE_FLASH))
	{
		if (!FNullEnt(pentToSkip))
        {
            auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pentToSkip));

            if (Player)
            {
                if (Player->IsAlive())
                {
                    if (Player->pev->flags & FL_ONGROUND)
                    {
                        if (Player->m_pActiveItem)
                        {
                            if ((Player->m_pActiveItem->iItemSlot() == PRIMARY_WEAPON_SLOT) || (Player->m_pActiveItem->iItemSlot() == PISTOL_SLOT))
                            {
                                auto Distance = MAX_SPECTATOR_ID_RANGE;

                                if ((Player->m_pActiveItem->m_iId == WEAPON_XM1014) || (Player->m_pActiveItem->m_iId == WEAPON_M3))
                                {
                                    Distance = (MAX_ID_RANGE / 2.0f);
                                }

                                auto trEnd = this->GetUserAiming(pentToSkip, Distance);

                                if (!FNullEnt(trEnd.pHit))
                                {
                                    g_engfuncs.pfnMakeVectors(pentToSkip->v.v_angle);

                                    auto vEndResult = (Vector)vStart + gpGlobals->v_forward * 9999.0f;

                                    g_engfuncs.pfnTraceLine(vStart, vEndResult, fNoMonsters, pentToSkip, pTraceResult);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

TraceResult CPugTraceLine::GetUserAiming(edict_t *pEntity, float fDistanceLimit)
{
	TraceResult trEnd;

	if (!FNullEnt(pEntity))
    {
        Vector vForward;

        Vector vSource = pEntity->v.origin + pEntity->v.view_ofs;

        g_engfuncs.pfnAngleVectors(pEntity->v.v_angle, vForward, NULL, NULL);

        Vector vDest = vSource + vForward * fDistanceLimit;
        
        g_engfuncs.pfnTraceLine(vSource, vDest, dont_ignore_monsters, pEntity, &trEnd);
	}

	return trEnd;
}