#pragma once

class CMetaEngine
{
public:
    enginefuncs_t m_FunctionTable_Pre;
    enginefuncs_t m_FunctionTable_Post;

#pragma region ENGINE_PRE
#pragma endregion

#pragma region ENGINE_POST
    static void POST_TraceLine(const float* start, const float* end, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr);
#pragma endregion
};

extern CMetaEngine gMetaEngine;