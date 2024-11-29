#pragma once

class CMetaDLL
{
public:
    DLL_FUNCTIONS m_FunctionTable_Pre;
    DLL_FUNCTIONS m_FunctionTable_Post;

    NEW_DLL_FUNCTIONS m_NewFunctionTable_Pre;
    NEW_DLL_FUNCTIONS m_NewFunctionTable_Post;

#pragma region DLL_PRE
    static void PRE_ClientDisconnect(edict_t *pEntity);
    static void PRE_ClientCommand(edict_t *pEntity);
#pragma endregion

#pragma region DLL_POST
    static void POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
    static void POST_ServerDeactivate();
    static void POST_StartFrame();
    static void POST_ClientPutInServer(edict_t *pEntity);
#pragma endregion

#pragma region NEW_DLL_PRE
#pragma endregion

#pragma region NEW_DLL_POST
#pragma endregion
};

extern CMetaDLL gMetaDLL;