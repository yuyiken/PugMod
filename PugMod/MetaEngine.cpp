#include "precompiled.h"

CMetaEngine gMetaEngine;

#pragma region ENGINE_PRE
C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion)
{
	memset(&gMetaEngine.m_FunctionTable_Pre, 0, sizeof(enginefuncs_t));

	memcpy(pengfuncsFromEngine, &gMetaEngine.m_FunctionTable_Pre, sizeof(enginefuncs_t));

	return 1;
}
#pragma endregion

#pragma region ENGINE_POST
C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion)
{
	memset(&gMetaEngine.m_FunctionTable_Post, 0, sizeof(enginefuncs_t));

	gMetaEngine.m_FunctionTable_Post.pfnTraceLine = gMetaEngine.POST_TraceLine;

	memcpy(pengfuncsFromEngine, &gMetaEngine.m_FunctionTable_Post, sizeof(enginefuncs_t));

	return 1;
}

void CMetaEngine::POST_TraceLine(const float* start, const float* end, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr)
{
	gPugTraceLine.TraceLine(start, end, fNoMonsters, pentToSkip, ptr);

	RETURN_META(MRES_IGNORED);
}
#pragma endregion
