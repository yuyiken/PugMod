#pragma once

#define FTRACE_BULLET BIT(16)
#define FTRACE_FLASH BIT(17)
#define FTRACE_KNIFE BIT(18)

class CPugTraceLine
{
public:
    void TraceLine(const float *vStart, const float *vEnd, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
    TraceResult GetUserAiming(edict_t *pEntity, float DistanceLimit);
};

extern CPugTraceLine gPugTraceLine;