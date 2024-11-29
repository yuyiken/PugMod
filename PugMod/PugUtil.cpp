#include "precompiled.h"
#include "PugUtil.h"

CPugUtil gPugUtil;

void CPugUtil::ServerCommand(const char* Format, ...)
{
	static char szCmd[255] = { 0 };

	va_list	ArgPtr;

	va_start(ArgPtr, Format);

	vsnprintf(szCmd, sizeof(szCmd), Format, ArgPtr);

	va_end(ArgPtr);

	strcat(szCmd, "\n");

	g_engfuncs.pfnServerCommand(szCmd);
}

int CPugUtil::ParseColors(char* Buffer)
{
	size_t len = strlen(Buffer);

	int offs = 0;

	if (len > 0)
	{
		int c;

		for (size_t i = 0; i < len; i++)
		{
			c = Buffer[i];

			if (c == '^' && (i != len - 1))
			{
				c = Buffer[++i];

				if (c == 'n' || c == 't' || (c >= '1' && c <= '4'))
				{
					switch (c)
					{
						case '1': c = '\x01'; break;
						case '2': c = '\x02'; break;
						case '3': c = '\x03'; break;
						case '4': c = '\x04'; break;
						case 'n': c = '\n'; break;
						case 't': c = '\t'; break;
					}

					offs++;
				}
			}

			Buffer[i - offs] = c;
		}

		Buffer[len - offs] = '\0';
	}

	return offs;
}

void CPugUtil::SayText(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "SayText", NULL)))
	{
		char szText[191] = { 0 };

		va_list vArgList;
		va_start(vArgList, Format);
		Q_vsnprintf(szText, sizeof(szText), Format, vArgList);
		va_end(vArgList);

		if (Sender < PRINT_TEAM_BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = PRINT_TEAM_DEFAULT;
		}
		else if (Sender < PRINT_TEAM_DEFAULT)
		{
			Sender = abs(Sender) + MAX_CLIENTS;
		}

		this->ParseColors(szText);

		if (!FNullEnt(pEntity))
		{
			if (!(pEntity->v.flags & FL_FAKECLIENT))
			{
				g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pEntity);
				g_engfuncs.pfnWriteByte(Sender ? Sender : ENTINDEX(pEntity));
				g_engfuncs.pfnWriteString("%s");
				g_engfuncs.pfnWriteString(szText);
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
						g_engfuncs.pfnWriteString(szText);
						g_engfuncs.pfnMessageEnd();
					}
				}
			}
		}
	}
}

void CPugUtil::ClientPrint(edict_t* pEntity, int iMsgDest, const char* Format, ...)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}
	}

	va_list argList;

	va_start(argList, Format);

	char Buffer[188] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (iMsgDest == PRINT_CONSOLE)
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

	if (iMsgTextMsg || (iMsgTextMsg = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "TextMsg", NULL)))
	{
		if (!FNullEnt(pEntity))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgTextMsg, nullptr, pEntity);
		}
		else
		{
			g_engfuncs.pfnMessageBegin(MSG_BROADCAST, iMsgTextMsg, nullptr, nullptr);
		}

		g_engfuncs.pfnWriteByte(iMsgDest);
		g_engfuncs.pfnWriteString("%s");
		g_engfuncs.pfnWriteString(Buffer);
		g_engfuncs.pfnMessageEnd();
	}
}

unsigned short CPugUtil::FixedUnsigned16(float Value, float Scale)
{
	int Result = Value * Scale;

	if (Result < 0)
	{
		Result = 0;
	}

	if (Result > USHRT_MAX)
	{
		Result = USHRT_MAX;
	}

	return (unsigned short)Result;
}

short CPugUtil::FixedSigned16(float Value, float Scale)
{
	int Result = Value * Scale;

	if (Result > SHRT_MAX)
	{
		Result = SHRT_MAX;
	}

	if (Result < SHRT_MIN)
	{
		Result = SHRT_MIN;
	}

	return (short)Result;
}

hudtextparms_t CPugUtil::SetHud(vec3_t FromColor, vec3_t ToColor, float X, float Y, int Effect, float FxTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel)
{
	hudtextparms_t TextParams;

	TextParams.r1 = static_cast<byte>(ToColor.x);
	TextParams.g1 = static_cast<byte>(ToColor.y);
	TextParams.b1 = static_cast<byte>(ToColor.z);
	TextParams.a1 = 0xFF;

	TextParams.r2 = static_cast<byte>(FromColor.x);
	TextParams.g2 = static_cast<byte>(FromColor.y);
	TextParams.b2 = static_cast<byte>(FromColor.z);
	TextParams.a2 = 0xFF;

	TextParams.x = X;
	TextParams.y = Y;

	TextParams.effect = Effect;
	TextParams.fxTime = FxTime;
	TextParams.holdTime = HoldTime;
	TextParams.fadeinTime = FadeInTime;
	TextParams.fadeoutTime = FadeOutTime;
	TextParams.channel = Channel;

	return TextParams;
}

void CPugUtil::SendHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}
	}

	static char szString[511] = { 0 };

	va_list ArgPtr;

	va_start(ArgPtr, Format);

	vsnprintf(szString, sizeof(szString), Format, ArgPtr);

	va_end(ArgPtr);

	if (!FNullEnt(pEntity))
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, nullptr, pEntity);
	}
	else
	{
		g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_TEMPENTITY, nullptr, nullptr);
	}

	g_engfuncs.pfnWriteByte(TE_TEXTMESSAGE);
	g_engfuncs.pfnWriteByte(TextParams.channel & 0xFF);

	g_engfuncs.pfnWriteShort(this->FixedSigned16(TextParams.x, BIT(13)));
	g_engfuncs.pfnWriteShort(this->FixedSigned16(TextParams.y, BIT(13)));

	g_engfuncs.pfnWriteByte(TextParams.effect);

	g_engfuncs.pfnWriteByte(TextParams.r1);
	g_engfuncs.pfnWriteByte(TextParams.g1);
	g_engfuncs.pfnWriteByte(TextParams.b1);
	g_engfuncs.pfnWriteByte(TextParams.a1);

	g_engfuncs.pfnWriteByte(TextParams.r2);
	g_engfuncs.pfnWriteByte(TextParams.g2);
	g_engfuncs.pfnWriteByte(TextParams.b2);
	g_engfuncs.pfnWriteByte(TextParams.a2);

	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(TextParams.fadeinTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(TextParams.fadeoutTime, BIT(8)));
	g_engfuncs.pfnWriteShort(this->FixedUnsigned16(TextParams.holdTime, BIT(8)));

	if (TextParams.effect == 2)
	{
		g_engfuncs.pfnWriteShort(this->FixedUnsigned16(TextParams.fxTime, BIT(8)));
	}

	g_engfuncs.pfnWriteString(szString);
	g_engfuncs.pfnMessageEnd();
}

void CPugUtil::SendDHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}
	}

	static char Text[128] = {0};

	va_list ArgPtr;

	va_start(ArgPtr, Format);

	int Length = vsnprintf(Text, sizeof(Text), Format, ArgPtr);

	va_end(ArgPtr);

	if (!FNullEnt(pEntity))
	{
		g_engfuncs.pfnMessageBegin(MSG_ONE_UNRELIABLE, SVC_DIRECTOR, nullptr, pEntity);
	}
	else
	{
		g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_DIRECTOR, nullptr, nullptr);
	}

	g_engfuncs.pfnWriteByte(Length + 31);
	g_engfuncs.pfnWriteByte(6);
	g_engfuncs.pfnWriteByte(TextParams.effect);
	g_engfuncs.pfnWriteLong(TextParams.b1 + (TextParams.g1 << 8) + (TextParams.r1 << 16));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.x)));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.y)));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.fadeinTime)));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.fadeoutTime)));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.holdTime)));
	g_engfuncs.pfnWriteLong((*((int32_t *)&TextParams.fxTime)));
	g_engfuncs.pfnWriteString(Text);
	g_engfuncs.pfnMessageEnd();
}

void CPugUtil::ScreenFade(edict_t *pEntity, unsigned short Duration, unsigned short HoldTime, short FadeFlags, short Red, short Green, short Blue, short Alpha)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}

		static int iMsgScreenFade;

		if (iMsgScreenFade || (iMsgScreenFade = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ScreenFade", NULL)))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgScreenFade, nullptr, pEntity);
			g_engfuncs.pfnWriteShort(Duration);
			g_engfuncs.pfnWriteShort(HoldTime);
			g_engfuncs.pfnWriteShort(FadeFlags);
			g_engfuncs.pfnWriteByte(Red);
			g_engfuncs.pfnWriteByte(Green);
			g_engfuncs.pfnWriteByte(Blue);
			g_engfuncs.pfnWriteByte(Alpha);
			g_engfuncs.pfnMessageEnd();
		}
	}
}

void CPugUtil::TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}

		static int iMsgTeamInfo;

		if (iMsgTeamInfo || (iMsgTeamInfo = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "TeamInfo", NULL)))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgTeamInfo, nullptr, pEntity);
			g_engfuncs.pfnWriteByte(playerIndex);
			g_engfuncs.pfnWriteString(pszTeamName);
			g_engfuncs.pfnMessageEnd();
		}
	}
}

void CPugUtil::SendDeathMsg(edict_t *pEntity, CBaseEntity *pKiller, CBasePlayer *pVictim, CBasePlayer *pAssister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->v.flags & FL_FAKECLIENT)
		{
			return;
		}
	}

	static int iDeathMsg;
	
	if (iDeathMsg || (iDeathMsg = gpMetaUtilFuncs->pfnGetUserMsgID(&Plugin_info, "DeathMsg", NULL)))
	{
		if (!FNullEnt(pEntity))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iDeathMsg, nullptr, pEntity);
		}
		else
		{
			g_engfuncs.pfnMessageBegin(MSG_ALL, iDeathMsg, nullptr, nullptr);
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

std::vector<CBasePlayer *> CPugUtil::GetPlayers(bool InGame, bool Bots)
{
	std::vector<CBasePlayer *> Players = {};

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				if ((Player->edict()->v.flags & FL_PROXY) == FL_PROXY)
				{
					continue;
				}

				if (InGame)
				{
					if ((Player->m_iTeam != TERRORIST) && (Player->m_iTeam != CT))
					{
						continue;
					}
				}

				if (!Bots)
				{
					if (Player->IsBot())
					{
						continue;
					}
				}

				Players.push_back(Player);
			}
		}
	}

	return Players;
}
