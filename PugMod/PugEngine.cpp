#include "precompiled.h"

CPugEngine gPugEngine;

void CPugEngine::RegisterHook(const char* MsgName, bool (*Function)(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity))
{
	auto UserMsgId = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, MsgName, 0);

	if (UserMsgId)
	{
		this->m_Hook[UserMsgId].Function = Function;
	}
}

bool CPugEngine::MessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity)
{
	if (this->m_Hook.find(msg_type) != this->m_Hook.end())
	{
		this->m_MsgId = msg_type;

		this->m_Data.msg_dest = msg_dest;

		this->m_Data.msg_type = msg_type;

		this->m_Data.pOrigin = pOrigin;

		this->m_Data.pEntity = pEntity;

		this->m_Data.Param.clear();

		return true;
	}

	this->m_MsgId = 0;

	return false;
}

bool CPugEngine::MessageEnd()
{
	if (this->m_MsgId)
	{
		auto Hook = this->m_Hook.find(this->m_MsgId);

		if (Hook != this->m_Hook.end())
		{
			if (Hook->second.Function)
			{
				auto BlockMessage = ((bool(*)(int msg_dest, int msg_type, const float* pOrigin, edict_t * pEntity))Hook->second.Function)(this->m_Data.msg_dest, this->m_Data.msg_type, this->m_Data.pOrigin, this->m_Data.pEntity);

				if (!BlockMessage)
				{
					MESSAGE_BEGIN(this->m_Data.msg_dest, this->m_Data.msg_type, this->m_Data.pOrigin, this->m_Data.pEntity);

					for (auto const& Param : this->m_Data.Param)
					{
						switch (Param.second.Type)
						{
							case MESSAGE_TYPE_BYTE:
							{
								WRITE_BYTE(Param.second.iValue);
								break;
							}
							case MESSAGE_TYPE_CHAR:
							{
								WRITE_CHAR(Param.second.iValue);
								break;
							}
							case MESSAGE_TYPE_SHORT:
							{
								WRITE_SHORT(Param.second.iValue);
								break;
							}
							case MESSAGE_TYPE_LONG:
							{
								WRITE_LONG(Param.second.iValue);
								break;
							}
							case MESSAGE_TYPE_ANGLE:
							{
								WRITE_ANGLE(Param.second.flValue);
								break;
							}
							case MESSAGE_TYPE_COORD:
							{
								WRITE_COORD(Param.second.flValue);
								break;
							}
							case MESSAGE_TYPE_STRING:
							{
								WRITE_STRING(Param.second.szValue);
								break;
							}
							case MESSAGE_TYPE_ENTITY:
							{
								WRITE_ENTITY(Param.second.iValue);
								break;
							}
						}
					}

					MESSAGE_END();
				}
			}
		}

		this->m_MsgId = 0;

		return true;
	}

	return false;
}

bool CPugEngine::WriteByte(int iValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_BYTE;
		Param.iValue = iValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteChar(int iValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_CHAR;
		Param.iValue = iValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteShort(int iValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_SHORT;
		Param.iValue = iValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteLong(int iValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_LONG;
		Param.iValue = iValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteAngle(float flValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_ANGLE;
		Param.flValue = flValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteCoord(float flValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_COORD;
		Param.flValue = flValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteString(const char* szValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_STRING;

		Q_strcpy(Param.szValue, szValue);

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

bool CPugEngine::WriteEntity(int iValue)
{
	if (this->m_MsgId)
	{
		P_MESSAGE_PARAM Param = { 0 };

		Param.Type = MESSAGE_TYPE_ENTITY;
		Param.iValue = iValue;

		this->m_Data.Param[this->m_Data.Param.size()] = Param;

		return true;
	}

	return false;
}

int CPugEngine::GetByte(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_BYTE)
		{
			return Param->second.iValue;
		}
	}

	return  0;
}

int CPugEngine::GetChar(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_CHAR)
		{
			return Param->second.iValue;
		}
	}

	return  0;
}

int CPugEngine::GetShort(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_SHORT)
		{
			return Param->second.iValue;
		}
	}

	return  0;
}

int CPugEngine::GetLong(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_LONG)
		{
			return Param->second.iValue;
		}
	}

	return  0;
}

float CPugEngine::GetAngle(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_ANGLE)
		{
			return Param->second.flValue;
		}
	}

	return  0.0f;
}

float CPugEngine::GetCoord(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_COORD)
		{
			return Param->second.flValue;
		}
	}

	return  0.0f;
}

const char* CPugEngine::GetString(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_STRING)
		{
			return Param->second.szValue;
		}
	}

	return  nullptr;
}

int CPugEngine::GetEntity(int Id)
{
	auto Param = this->m_Data.Param.find(Id);

	if (Param != this->m_Data.Param.end())
	{
		if (Param->second.Type == MESSAGE_TYPE_ENTITY)
		{
			return Param->second.iValue;
		}
	}

	return  0;
}

void CPugEngine::SetArgInt(int Id, int iValue)
{
	if (this->m_Data.Param.find(Id) != this->m_Data.Param.end())
	{
		this->m_Data.Param[Id].iValue = iValue;
	}
}

void CPugEngine::SetArgFloat(int Id, float flValue)
{
	if (this->m_Data.Param.find(Id) != this->m_Data.Param.end())
	{
		this->m_Data.Param[Id].flValue = flValue;
	}
}

void CPugEngine::SetArgString(int Id, const char* szValue)
{
	if (this->m_Data.Param.find(Id) != this->m_Data.Param.end())
	{
		Q_strcpy(this->m_Data.Param[Id].szValue, szValue);
	}
}