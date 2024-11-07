#include "precompiled.h"

CPugUtil gPugUtil;

void CPugUtil::ServerCommand(const char* Format, ...)
{
	char Command[255] = { 0 };

	va_list	ArgPtr;

	va_start(ArgPtr, Format);

	vsnprintf(Command, sizeof(Command), Format, ArgPtr);

	va_end(ArgPtr);

	Q_strncat(Command, "\n", 1);

	g_engfuncs.pfnServerCommand(Command);
}

void CPugUtil::ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[188] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (msg_dest == PRINT_CONSOLE)
	{
		if (Length > 125)
		{
			Length = 125;
		}

		Buffer[Length++] = '\n';
		Buffer[Length++] = '\n';
		Buffer[Length] = 0;
	}

	static int iMsgTextMsg;

	if (iMsgTextMsg || (iMsgTextMsg = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "TextMsg", NULL)))
	{
		if (pEntity)
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgTextMsg, NULL, pEntity);
		}
		else
		{
			g_engfuncs.pfnMessageBegin(MSG_BROADCAST, iMsgTextMsg, NULL, NULL);
		}

		g_engfuncs.pfnWriteByte(msg_dest);
		g_engfuncs.pfnWriteString("%s");
		g_engfuncs.pfnWriteString(Buffer);
		g_engfuncs.pfnMessageEnd();
	}
}

int CPugUtil::ParseColors(char* Buffer)
{
	size_t len = strlen(Buffer);

	int offs = 0;

	if (len > 0)
	{
		char c = '\0';

		for (size_t i = 0; i < len; i++)
		{
			c = Buffer[i];

			if (c == '^' && (i != len - 1))
			{
				c = Buffer[++i];

				switch (c)
				{
					case '1': // Default chat text color
					{
						c = '\x01';
						offs++;
						break;
					}
					case '2': // ?
					{
						c = '\x02';
						offs++;
						break;
					}
					case '3': // ?
					{
						c = '\x03';
						offs++;
						break;
					}
					case '4': // Green chat text color
					{
						c = '\x04';
						offs++;
						break;
					}
					case 'n': // New Line Character
					{
						c = '\n';
						offs++;
						break;
					}
					case 't': // TAB Character
					{
						c = '\t';
						offs++;
						break;
					}
				}
			}

			Buffer[i - offs] = c;
		}

		Buffer[len - offs] = '\0';
	}

	return offs;
}

void CPugUtil::ReplaceAll(std::string& String, const std::string& From, const std::string& To)
{
	if (!From.empty())
	{
		size_t StartPos = 0;

		while ((StartPos = String.find(From, StartPos)) != std::string::npos)
		{
			String.replace(StartPos, From.length(), To);

			StartPos += To.length();
		}
	}
}

void CPugUtil::SayText(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "SayText", NULL)))
	{
		char Buffer[191] = { 0 };

		va_list vArgList;
		va_start(vArgList, Format);
		Q_vsnprintf(Buffer, sizeof(Buffer), Format, vArgList);
		va_end(vArgList);

		if (Sender < PRINT_TEAM_BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = PRINT_TEAM_DEFAULT;
		}
		else if (Sender < PRINT_TEAM_DEFAULT)
		{
			Sender = abs(Sender) + MAX_CLIENTS;
		}

		this->ParseColors(Buffer);

		if (!FNullEnt(pEntity))
		{
			if (!(pEntity->v.flags & FL_FAKECLIENT))
			{
				g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pEntity);
				g_engfuncs.pfnWriteByte(Sender ? Sender : ENTINDEX(pEntity));
				g_engfuncs.pfnWriteString("%s");
				g_engfuncs.pfnWriteString(Buffer);
				g_engfuncs.pfnMessageEnd();
			}
		}
		else
		{
			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				edict_t* pTempEntity = INDEXENT(i);

				if (!FNullEnt(pTempEntity))
				{
					if (!(pTempEntity->v.flags & FL_FAKECLIENT))
					{
						g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pTempEntity);
						g_engfuncs.pfnWriteByte(Sender ? Sender : i);
						g_engfuncs.pfnWriteString("%s");
						g_engfuncs.pfnWriteString(Buffer);
						g_engfuncs.pfnMessageEnd();
					}
				}
			}
		}
	}
}

unsigned short CPugUtil::FixedUnsigned16(float value, float scale)
{
	int output = value * scale;

	if (output < 0)
	{
		output = 0;
	}

	if (output > USHRT_MAX)
	{
		output = USHRT_MAX;
	}

	return (unsigned short)output;
}

short CPugUtil::FixedSigned16(float value, float scale)
{
	int output = value * scale;

	if (output > SHRT_MAX)
	{
		output = SHRT_MAX;
	}

	if (output < SHRT_MIN)
	{
		output = SHRT_MIN;
	}

	return (short)output;
}

hudtextparms_t CPugUtil::HudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel)
{
	hudtextparms_t hud;

	hud.r1 = static_cast<byte>(red);
	hud.g1 = static_cast<byte>(green);
	hud.b1 = static_cast<byte>(blue);
	hud.a1 = 0xFF;
	hud.r2 = 0xFF;
	hud.g2 = 0xFF;
	hud.b2 = 0xFF;
	hud.a2 = 0xFF;
	hud.x = x;
	hud.y = y;
	hud.effect = effects;
	hud.fxTime = fxtime;
	hud.holdTime = holdtime;
	hud.fadeinTime = fadeintime;
	hud.fadeoutTime = fadeouttime;
	hud.channel = channel;

	return hud;
}

void CPugUtil::HudMessage(edict_t* pEntity, hudtextparms_t textparms, const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[511] = { 0 };

	vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	this->ParseColors(Buffer);

	if (pEntity)
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, pEntity);
	}
	else
	{
		g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_TEMPENTITY, NULL, NULL);
	}

	g_engfuncs.pfnWriteByte(TE_TEXTMESSAGE);
	g_engfuncs.pfnWriteByte(textparms.channel & 0xFF);

	g_engfuncs.pfnWriteShort(this->FixedSigned16(textparms.x, BIT(13)));
	g_engfuncs.pfnWriteShort(this->FixedSigned16(textparms.y, BIT(13)));

	g_engfuncs.pfnWriteByte(textparms.effect);

	g_engfuncs.pfnWriteByte(textparms.r1);
	g_engfuncs.pfnWriteByte(textparms.g1);
	g_engfuncs.pfnWriteByte(textparms.b1);
	g_engfuncs.pfnWriteByte(textparms.a1);

	g_engfuncs.pfnWriteByte(textparms.r2);
	g_engfuncs.pfnWriteByte(textparms.g2);
	g_engfuncs.pfnWriteByte(textparms.b2);
	g_engfuncs.pfnWriteByte(textparms.a2);

	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fadeinTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fadeoutTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.holdTime, BIT(8)));

	if (textparms.effect == 2)
	{
		g_engfuncs.pfnWriteShort(this->FixedUnsigned16(textparms.fxTime, BIT(8)));
	}

	g_engfuncs.pfnWriteString(Buffer);
	g_engfuncs.pfnMessageEnd();
}

void CPugUtil::TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName)
{
	static int iMsgTeamInfo;

	if (iMsgTeamInfo || (iMsgTeamInfo = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "TeamInfo", NULL)))
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgTeamInfo, nullptr, pEntity);
		g_engfuncs.pfnWriteByte(playerIndex);
		g_engfuncs.pfnWriteString(pszTeamName);
		g_engfuncs.pfnMessageEnd();
	}
}

void CPugUtil::DeathMsg(edict_t* pEntity, CBaseEntity* pKiller, CBasePlayer* pVictim, CBasePlayer* pAssister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	static int iDeathMsg;
	
	if (iDeathMsg || (iDeathMsg = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "DeathMsg", NULL)))
	{
		if (FNullEnt(pEntity))
		{
			g_engfuncs.pfnMessageBegin(MSG_ALL, iDeathMsg, nullptr, nullptr);
		}
		else
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iDeathMsg, nullptr, pEntity);
		}

		g_engfuncs.pfnWriteByte((pKiller && pKiller->IsPlayer()) ? pKiller->entindex() : 0);
		g_engfuncs.pfnWriteByte(pVictim->entindex());
		g_engfuncs.pfnWriteByte((iRarityOfKill & 0x001));
		g_engfuncs.pfnWriteString(killerWeaponName);

		if (iDeathMessageFlags > 0)
		{
			g_engfuncs.pfnWriteLong(iDeathMessageFlags);

			if (iDeathMessageFlags & 0x001)
			{
				g_engfuncs.pfnWriteCoord(pVictim->pev->origin.x);
				g_engfuncs.pfnWriteCoord(pVictim->pev->origin.y);
				g_engfuncs.pfnWriteCoord(pVictim->pev->origin.z);
			}

			if (iDeathMessageFlags & 0x002)
			{
				g_engfuncs.pfnWriteByte((pAssister && pAssister->IsPlayer()) ? pAssister->entindex() : 0);
			}

			if (iDeathMessageFlags & 0x004)
			{
				g_engfuncs.pfnWriteByte(iRarityOfKill);
			}
		}

		g_engfuncs.pfnMessageEnd();
	}
}
