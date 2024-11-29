#include "precompiled.h"

CPugTraceLine gPugTraceLine;

void CPugTraceLine::TraceLine(const float *vStart, const float *vEnd, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	if ((fNoMonsters == dont_ignore_monsters) && (gpGlobals->trace_flags != FTRACE_FLASH))
	{
		if (!FNullEnt(pentToSkip))
        {
            auto Player = UTIL_PlayerByIndexSafe(g_engfuncs.pfnIndexOfEdict(pentToSkip));

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

                                auto trResult = this->GetUserAiming(pentToSkip, Distance);

                                if (!FNullEnt(trResult.pHit))
                                {
                                    g_engfuncs.pfnMakeVectors(pentToSkip->v.v_angle);

                                    auto vEndRes = (Vector)vStart + gpGlobals->v_forward * 9999.0f;

                                    g_engfuncs.pfnTraceLine(vStart, vEndRes, fNoMonsters, pentToSkip, ptr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

TraceResult CPugTraceLine::GetUserAiming(edict_t *pEntity, float DistanceLimit)
{
    TraceResult Result = {};

    if (!FNullEnt(pEntity))
    {
        Vector v_forward;

        Vector v_src = pEntity->v.origin + pEntity->v.view_ofs;

        g_engfuncs.pfnAngleVectors(pEntity->v.v_angle, v_forward, nullptr, nullptr);

        Vector v_dest = v_src + v_forward * DistanceLimit;

        g_engfuncs.pfnTraceLine(v_src, v_dest, dont_ignore_monsters, pEntity, &Result);
    }

    return Result;
}
