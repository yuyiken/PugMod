#include "precompiled.h"

CMetaDLL gMetaDLL;

#pragma region META_DLL_PRE
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	memset(&gMetaDLL.m_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));

	gMetaDLL.m_FunctionTable_Pre.pfnClientDisconnect = gMetaDLL.PRE_ClientDisconnect;

	gMetaDLL.m_FunctionTable_Pre.pfnClientCommand = gMetaDLL.PRE_ClientCommand;

	memcpy(pFunctionTable, &gMetaDLL.m_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));

	return 1;
}

void CMetaDLL::PRE_ClientDisconnect(edict_t *pEntity)
{
	gPugPlayer.ClientDisconnect(pEntity);

	RETURN_META(MRES_IGNORED);
}

void CMetaDLL::PRE_ClientCommand(edict_t *pEntity)
{
	if (gPugPlayer.ClientCommand(pEntity))
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region META_DLL_POST
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	memset(&gMetaDLL.m_FunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));

	gMetaDLL.m_FunctionTable_Post.pfnServerActivate = gMetaDLL.POST_ServerActivate;

	gMetaDLL.m_FunctionTable_Post.pfnServerDeactivate = gMetaDLL.POST_ServerDeactivate;

	gMetaDLL.m_FunctionTable_Post.pfnStartFrame = gMetaDLL.POST_StartFrame;

	gMetaDLL.m_FunctionTable_Post.pfnClientPutInServer = gMetaDLL.POST_ClientPutInServer;

	memcpy(pFunctionTable, &gMetaDLL.m_FunctionTable_Post, sizeof(DLL_FUNCTIONS));

	return 1;
}

void CMetaDLL::POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	gPugTask.ServerActivate();

	gPugCvar.ServerActivate();

	gPugDeathmatch.ServerActivate();

	gPugServerCmd.ServerActivate();

	gPugMod.ServerActivate();

	RETURN_META(MRES_IGNORED);
}

void CMetaDLL::POST_ServerDeactivate()
{
	gPugTask.ServerDeactivate();

	gPugMod.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}

void CMetaDLL::POST_StartFrame()
{
	gPugTask.StartFrame();

	gPugMod.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}

void CMetaDLL::POST_ClientPutInServer(edict_t *pEntity)
{
	gPugPlayer.PutInServer(pEntity);
	
	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region NEW_DLL_PRE
C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	memset(&gMetaDLL.m_NewFunctionTable_Pre, 0, sizeof(NEW_DLL_FUNCTIONS));

	// Here

	memcpy(pNewFunctionTable, &gMetaDLL.m_NewFunctionTable_Pre, sizeof(NEW_DLL_FUNCTIONS));

	return 1;
}
#pragma endregion

#pragma region NEW_DLL_POST
C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	memset(&gMetaDLL.m_NewFunctionTable_Post, 0, sizeof(NEW_DLL_FUNCTIONS));

	// Here

	memcpy(pNewFunctionTable, &gMetaDLL.m_NewFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));

	return 1;
}
#pragma endregion