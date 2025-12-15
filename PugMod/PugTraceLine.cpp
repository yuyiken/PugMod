#include "precompiled.h"

CPugTraceLine gPugTraceLine;

void CPugTraceLine::TraceLine(const float* vFrom, const float* vTo, int fNoMonsters, edict_t* pentToSkip, TraceResult* pTraceResult)
{
	if (fNoMonsters == dont_ignore_monsters) 
	{
		if (!FNullEnt(pentToSkip))
		{
            auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pentToSkip));

            if (Player)
            {
                if (Player->IsAlive())
                {
                    if (Player->m_pActiveItem)
                    {
                        if ((Player->m_pActiveItem->iItemSlot() == PRIMARY_WEAPON_SLOT) || (Player->m_pActiveItem->iItemSlot() == PISTOL_SLOT))
                        {
                            if (!(Player->pev->flags & FL_ONGROUND))
                            {
                                return;
                            }

                            if (Player->pev->velocity.Length2D() > (Player->pev->maxspeed / 2.0f))
                            {
                                return;
                            }

                            if (Player->pev->punchangle.Length2D() > 1.0f)
                            {
                                return;
                            }

                            if (Player->m_pActiveItem->m_iId == WEAPON_XM1014 || Player->m_pActiveItem->m_iId == WEAPON_M3)
                            {
                                return;
                            }
                            
                            if (Player->m_pActiveItem->m_iId == WEAPON_SCOUT || Player->m_pActiveItem->m_iId == WEAPON_SG550 || Player->m_pActiveItem->m_iId == WEAPON_AWP || Player->m_pActiveItem->m_iId == WEAPON_G3SG1)
                            {
                                if (!Player->m_bResumeZoom)
                                {
                                    return;
                                }
                            }

                            auto trResult = this->GetUserAiming(pentToSkip, 2000.0f);

                            if (!FNullEnt(trResult.pHit))
                            {
                                auto Target = UTIL_PlayerByIndexSafe(ENTINDEX(trResult.pHit));

                                if (Target)
                                {
                                    g_engfuncs.pfnMakeVectors(pentToSkip->v.v_angle);

                                    auto vToResult = (Vector)vFrom + gpGlobals->v_forward * 8192.0f;

                                    g_engfuncs.pfnTraceLine(vFrom, vToResult, fNoMonsters, pentToSkip, pTraceResult);
                                }
                            }
                        }
                    }
                }
            }
		}
	}
}

TraceResult CPugTraceLine::GetUserAiming(edict_t* pEntity, float DistanceLimit)
{
	TraceResult Result = { };

	if (!FNullEnt(pEntity))
    {
        Vector v_forward;

        Vector v_src = pEntity->v.origin + pEntity->v.view_ofs;

        g_engfuncs.pfnAngleVectors(pEntity->v.v_angle, v_forward, NULL, NULL);

        Vector v_dest = v_src + v_forward * DistanceLimit;
        
        g_engfuncs.pfnTraceLine(v_src, v_dest, 0, pEntity, &Result);
	}

	return Result;
}