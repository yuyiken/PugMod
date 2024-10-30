#include "precompiled.h"

// DLL PRE Functions Table
DLL_FUNCTIONS gDLL_FunctionTable_Pre;

// DLL POST Functions Table
DLL_FUNCTIONS gDLL_FunctionTable_Post;

#pragma region DLL_PRE
C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	memset(&gDLL_FunctionTable_Pre, 0, sizeof(DLL_FUNCTIONS));

	// Register Functions Here

	memcpy(pFunctionTable, &gDLL_FunctionTable_Pre, sizeof(DLL_FUNCTIONS));

	return 1;
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

	gDLL_FunctionTable_Post.pfnClientKill = DLL_POST_ClientKill;

	gDLL_FunctionTable_Post.pfnClientUserInfoChanged = DLL_POST_ClientUserInfoChanged;

	gDLL_FunctionTable_Post.pfnClientCommand = DLL_POST_ClientCommand;	

	memcpy(pFunctionTable, &gDLL_FunctionTable_Post, sizeof(DLL_FUNCTIONS));

	return 1;
}

void DLL_POST_ServerActivate(edict_t* pEdictList, int edictCount, int clientMax)
{
	gPugConfig.ServerActivate();

	gPugCurl.ServerActivate();

	gPugTask.ServerActivate();

	gPugMod.ServerActivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ServerDeactivate(void)
{
	gPugCurl.ServerDeactivate();

	gPugTask.ServerDeactivate();

	gPugMod.ServerDeactivate();

	RETURN_META(MRES_IGNORED);
}

void DLL_POST_StartFrame()
{
	gPugCurl.StartFrame();

	gPugTask.ServerFrame();

	RETURN_META(MRES_IGNORED);
}

qboolean DLL_POST_ClientConnect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	RETURN_META_VALUE(MRES_IGNORED, TRUE);
}

void DLL_POST_ClientPutInServer(edict_t* pEntity)
{
	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ClientKill(edict_t* pEntity)
{
	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ClientUserInfoChanged(edict_t* pEntity, char* InfoBuffer)
{
	RETURN_META(MRES_IGNORED);
}

void DLL_POST_ClientCommand(edict_t* pEntity)
{
	if (pEntity)
	{
		if (!FNullEnt(pEntity))
		{

		}
	}

	RETURN_META(MRES_IGNORED);
}
#pragma endregion
