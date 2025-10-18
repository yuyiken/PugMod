#pragma once

constexpr auto MESSAGE_TYPE_BYTE = 0;
constexpr auto MESSAGE_TYPE_CHAR = 1;
constexpr auto MESSAGE_TYPE_SHORT = 2;
constexpr auto MESSAGE_TYPE_LONG = 3;
constexpr auto MESSAGE_TYPE_ANGLE = 4;
constexpr auto MESSAGE_TYPE_COORD = 5;
constexpr auto MESSAGE_TYPE_STRING = 6;
constexpr auto MESSAGE_TYPE_ENTITY = 7;

struct MESSAGE_BEGIN_FUNC
{
	bool (*Function)(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity);
};

typedef struct MESSAGE_PARAM_S
{
	int Type;
	int iValue;
	float flValue;
	char szValue[200];
} P_MESSAGE_PARAM, * LP_MESSAGE_PARAM;

typedef struct MESSAGE_DATA_S
{
	int msg_dest;
	int msg_type;
	const float* pOrigin;
	edict_t* pEntity;
	std::map<int, P_MESSAGE_PARAM> Param;
} P_MESSAGE_DATA, * LP_MESSAGE_DATA;

class CPugEngine
{
public:
	void RegisterHook(const char* MsgName, bool (*Function)(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity));

	bool MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity);
	bool MessageEnd();

	bool WriteByte(int iValue);
	bool WriteChar(int iValue);
	bool WriteShort(int iValue);
	bool WriteLong(int iValue);
	bool WriteAngle(float flValue);
	bool WriteCoord(float flValue);
	bool WriteString(const char* szValue);
	bool WriteEntity(int iValue);

	int GetByte(int Id);
	int GetChar(int Id);
	int GetShort(int Id);
	int GetLong(int Id);
	float GetAngle(int Id);
	float GetCoord(int Id);
	const char* GetString(int Id);
	int GetEntity(int Id);

	void SetArgInt(int Id, int iValue);
	void SetArgFloat(int Id, float flValue);
	void SetArgString(int Id, const char* szValue);

private:
	int m_MsgId = 0;

	P_MESSAGE_DATA m_Data = { 0 };

	std::map<int, MESSAGE_BEGIN_FUNC> m_Hook;
};

extern CPugEngine gPugEngine;