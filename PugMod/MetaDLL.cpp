#include "precompiled.h"

// DLL PRE Functions Table
DLL_FUNCTIONS gDLL_FunctionTable_Pre;

// DLL POST Functions Table
DLL_FUNCTIONS gDLL_FunctionTable_Post;

#pragma region DLL_PRE
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	memset(&gDLL_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));

	gDLL_FunctionTable_Pre.pfnClientCommand = DLL_PRE_ClientCommand;

	memcpy(pFunctionTable, &gDLL_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_PRE_ClientCommand(edict_t* pEntity)
{
	if (gPugClientCmd.Command(pEntity))
	{
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion

#pragma region DLL_POST
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	memset(&gDLL_FunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));

	gDLL_FunctionTable_Post.pfnServerActivate = DLL_POST_ServerActivate;

	gDLL_FunctionTable_Post.pfnServerDeactivate = DLL_POST_ServerDeactivate;

	gDLL_FunctionTable_Post.pfnStartFrame = DLL_POST_StartFrame;

	gDLL_FunctionTable_Post.pfnClientConnect = DLL_POST_ClientConnect;

	gDLL_FunctionTable_Post.pfnClientPutInServer = DLL_POST_ClientPutInServer;

	memcpy(pFunctionTable, &gDLL_FunctionTable_Post, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_POST_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	gPugCvar.ServerActivate();

	gPugCurl.ServerActivate();

	gPugTask.ServerActivate();

	gPugPlayer.ServerActivate();

	gPugMod.ServerActivate();

	gPugDeathmatch.ServerActivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ServerDeactivate(void)
{
	gPugCurl.ServerDeactivate();

	gPugTask.ServerDeactivate();

	gPugMod.ServerDeactivate();

	gPugDeathmatch.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_StartFrame()
{
	gPugCurl.StartFrame();

	gPugTask.ServerFrame();

	RETURN_META(MRES_IGNORED);
}

BOOL DLL_POST_ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	gPugPlayer.Connect(pEntity, pszName, pszAddress);

	RETURN_META_VALUE(MRES_IGNORED, TRUE);

	return TRUE;
}

void DLL_POST_ClientPutInServer(edict_t* pEntity)
{
	gPugPlayer.PutInServer(pEntity);

	RETURN_META(MRES_IGNORED);
}
#pragma endregion
