#pragma once

#pragma region DLL_PRE
void DLL_PRE_ClientCommand(edict_t *pEntity);
#pragma endregion

#pragma region DLL_POST
void DLL_POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
void DLL_POST_ServerDeactivate();
void DLL_POST_StartFrame();
void DLL_POST_ClientPutInServer(edict_t *pEntity);
void DLL_POST_ClientUserInfoChanged(edict_t *pEntity, char *infobuffer);
void DLL_POST_TraceLine();
#pragma endregion

#pragma region NEW_DLL_PRE
#pragma endregion

#pragma region NEW_DLL_POST
#pragma endregion

#pragma region ENGINE_PRE
void ENGINE_PRE_MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);
void ENGINE_PRE_WriteByte(int iValue);
void ENGINE_PRE_WriteChar(int iValue);
void ENGINE_PRE_WriteShort(int iValue);
void ENGINE_PRE_WriteLong(int iValue);
void ENGINE_PRE_WriteAngle(float flValue);
void ENGINE_PRE_WriteCoord(float flValue);
void ENGINE_PRE_WriteString(const char* sz);
void ENGINE_PRE_WriteEntity(int iValue);
void ENGINE_PRE_MessageEnd(void);
#pragma endregion

#pragma region ENGINE_POST
#pragma endregion