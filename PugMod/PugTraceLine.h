#pragma once

class CPugTraceLine
{
public:
    void TraceLine(const float* vFrom, const float* vTo, int fNoMonsters, edict_t* pentToSkip, TraceResult* pResult);
    TraceResult GetUserAiming(edict_t* pEntity, float DistanceLimit);
};

extern CPugTraceLine gPugTraceLine;