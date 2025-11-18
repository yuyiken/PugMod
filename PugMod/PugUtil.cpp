#include "precompiled.h"

CPugUtil gPugUtil;

const char* CPugUtil::GetPath()
{
	if (this->m_Path.empty())
	{
		std::string GameDir = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR);

		if (!GameDir.empty())
		{
			this->m_Path = gpMetaUtilFuncs->pfnGetPluginPath(PLID);

			if (!this->m_Path.empty())
			{
				this->m_Path.erase(0, GameDir.length() + 1U);

				std::replace(this->m_Path.begin(), this->m_Path.end(), (char)(92), (char)(47));

				this->m_Path.erase(this->m_Path.find_last_of((char)(47)), this->m_Path.length());

				while (std::count(this->m_Path.begin(), this->m_Path.end(), (char)(47)) > 1)
				{
					this->m_Path.erase(this->m_Path.find_last_of((char)(47)), this->m_Path.length());
				}
			}
		}
	}

	return this->m_Path.c_str();
}

const char* CPugUtil::GetFullPath()
{
	if (this->m_FullPath.empty())
	{
		std::string GameDir = gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR);

		if (!GameDir.empty())
		{
			this->m_FullPath = gpMetaUtilFuncs->pfnGetPluginPath(PLID);

			if (!this->m_FullPath.empty())
			{
				std::replace(this->m_FullPath.begin(), this->m_FullPath.end(), (char)(92), (char)(47));

				this->m_FullPath.erase(this->m_FullPath.find_last_of((char)(47)), this->m_FullPath.length());

				this->m_FullPath.erase(this->m_FullPath.find_last_of((char)(47)), this->m_FullPath.length());
			}
		}
	}

	return this->m_FullPath.c_str();
}

int CPugUtil::MakeDirectory(const char* Path)
{
	struct stat st = { };

	if (stat(Path, &st) == -1)
	{
#if defined(_WIN32)
		return _mkdir(Path);
#else
		return mkdir(Path, 0755);
#endif
	}

	return 0;
}

void CPugUtil::ServerCommand(const char *Format, ...)
{
	static char Command[255] = { 0 };

	va_list	ArgList;

	va_start(ArgList, Format);

	vsnprintf(Command, sizeof(Command), Format, ArgList);

	va_end(ArgList);

	strcat(Command, "\n");

	g_engfuncs.pfnServerCommand(Command);
}

bool CPugUtil::IsNetClient(edict_t *pEntity)
{
	if (!FNullEnt(pEntity))
	{
		if (pEntity->free == TRUE)
		{
			return false;
		}

		if (pEntity->pvPrivateData == NULL)
		{
			return false;
		}

		if ((pEntity->v.flags & FL_DORMANT) == FL_DORMANT)
		{
			return false;
		}

		if ((pEntity->v.flags & FL_PROXY) == FL_PROXY)
		{
			return false;
		}

		if ((pEntity->v.flags & FL_FAKECLIENT) == FL_FAKECLIENT)
		{
			return false;
		}

		return true;
	}

    return false;
}

void CPugUtil::PrintColor(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "SayText", NULL)))
	{
		char szText[191] = { 0 };

		va_list vArgList;

		va_start(vArgList, Format);

		vsnprintf(szText, sizeof(szText), Format, vArgList);

		va_end(vArgList);

		if (Sender < E_PRINT_TEAM::BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = E_PRINT_TEAM::DEFAULT;
		}
		else if (Sender < E_PRINT_TEAM::DEFAULT)
		{
			Sender = abs(Sender) + MAX_CLIENTS;
		}

		this->ParseColor(szText);

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
				pEntity = INDEXENT(i);

				if (this->IsNetClient(pEntity))
				{
					g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgSayText, nullptr, pEntity);
					g_engfuncs.pfnWriteByte(Sender ? Sender : i);
					g_engfuncs.pfnWriteString("%s");
					g_engfuncs.pfnWriteString(szText);
					g_engfuncs.pfnMessageEnd();
				}
			}
		}
	}
}

void CPugUtil::TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName)
{
	if (!FNullEnt(pEntity))
	{
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

void CPugUtil::ParseColor(char* Buffer)
{
	size_t Length = strlen(Buffer);

	int Offset = 0;

	if (Length > 0)
	{
		int Character;

		for (size_t i = 0; i < Length; i++)
		{
			Character = Buffer[i];

			if (Character == '^' && (i != Length - 1))
			{
				Character = Buffer[++i];

				if (Character == 'n' || Character == 't' || (Character >= '1' && Character <= '4'))
				{
					switch (Character)
					{
						case '1': Character = '\x01'; break;
						case '2': Character = '\x02'; break;
						case '3': Character = '\x03'; break;
						case '4': Character = '\x04'; break;
						case 'n': Character = '\n'; break;
						case 't': Character = '\t'; break;
					}

					Offset++;
				}
			}

			Buffer[i - Offset] = Character;
		}

		Buffer[Length - Offset] = '\0';
	}
}

void CPugUtil::ClientPrint(edict_t *pEntity, int iMsgDest, const char *Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[188] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (iMsgDest == E_PRINT::CONSOLE)
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

void CPugUtil::ClientCommand(edict_t *pEntity, const char *Format, ...)
{
	static char Command[256] = { 0 };

	va_list	ArgList;

	va_start(ArgList, Format);

	vsnprintf(Command, sizeof(Command), Format, ArgList);

	va_end(ArgList);

	strcat(Command, "\n");

	if (!FNullEnt(pEntity))
	{
		g_engfuncs.pfnClientCommand(pEntity, Command);
	}
	else
	{
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			pEntity = INDEXENT(i);

			if (this->IsNetClient(pEntity))
			{
				g_engfuncs.pfnClientCommand(pEntity, Command);
			}
		}
	}
}

void CPugUtil::ClientDrop(int EntityIndex, const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[255] = { 0 };

	vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (g_RehldsSvs && g_RehldsFuncs)
	{
		auto Gameclient = g_RehldsSvs->GetClient(EntityIndex - 1);

		if (Gameclient)
		{
			g_RehldsFuncs->DropClient(Gameclient, false, "%s", Buffer);
		}
	}
	else
	{
		auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

		if (Player)
		{
			int UserIndex = g_engfuncs.pfnGetPlayerUserId(Player->edict());

			if (!FNullEnt(Player->edict()) && UserIndex > 0)
			{
				if (strlen(Buffer) > 0)
				{
					this->ServerCommand("kick #%d %s", UserIndex, Buffer);
				}
				else
				{
					this->ServerCommand("kick #%d", UserIndex);
				}
			}

		}
	}
}

void CPugUtil::ClientSlap(edict_t *pEntity, float Damage, bool RandomDirection)
{
	if (pEntity->v.deadflag == DEAD_NO && pEntity->v.health > 0.0f)
	{
		if (pEntity->v.health > Damage)
		{
			if (RandomDirection)
			{
				pEntity->v.velocity.x += g_engfuncs.pfnRandomLong(-600, 600);
				pEntity->v.velocity.y += g_engfuncs.pfnRandomLong(-180, 180);
				pEntity->v.velocity.z += g_engfuncs.pfnRandomLong(100, 200);
			}
			else
			{
				Vector v_forward, v_right;
				Vector vang = pEntity->v.angles;

				float fang[3] = {0.0f, 0.0f, 0.0f};

				fang[0] = vang.x;
				fang[1] = vang.y;
				fang[2] = vang.z;

				g_engfuncs.pfnAngleVectors(fang, v_forward, v_right, NULL);

				pEntity->v.velocity = pEntity->v.velocity + v_forward * 220 + Vector(0.0f, 0.0f, 200.0f);
			}

			pEntity->v.punchangle.x = static_cast<vec_t>(g_engfuncs.pfnRandomLong(-10, 10));
			pEntity->v.punchangle.y = static_cast<vec_t>(g_engfuncs.pfnRandomLong(-10, 10));

			pEntity->v.health -= Damage;

			float armor = pEntity->v.armorvalue;

			armor -= Damage;

			if (armor < 0)
			{
				armor = 0;
			}

			pEntity->v.armorvalue = armor;

			pEntity->v.dmg_inflictor = pEntity;

			static const char *cs_sound[6] =
			{
				"player/pl_pain2.wav",
				"player/pl_pain4.wav",
				"player/pl_pain5.wav",
				"player/pl_pain6.wav",
				"player/pl_pain7.wav"
			};

			g_engfuncs.pfnEmitSound(pEntity, CHAN_VOICE, cs_sound[g_engfuncs.pfnRandomLong(0, 4)], 1.0, ATTN_NORM, 0, PITCH_NORM);
		}
		else
		{
			float Backup = pEntity->v.frags;

			MDLL_ClientKill(pEntity);

			pEntity->v.frags = Backup;
		}
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
	static char szString[511] = { 0 };

	va_list ArgPtr;

	va_start(ArgPtr, Format);

	vsnprintf(szString, sizeof(szString), Format, ArgPtr);

	va_end(ArgPtr);

	this->ParseColor(szString);

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
	static char Text[128] = {0};

	va_list ArgPtr;

	va_start(ArgPtr, Format);

	int Length = vsnprintf(Text, sizeof(Text), Format, ArgPtr);

	va_end(ArgPtr);

	this->ParseColor(Text);

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

void CPugUtil::SendDeathMsg(edict_t *pEntity, CBaseEntity *pKiller, CBasePlayer *pVictim, CBasePlayer *pAssister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	static int iDeathMsg;
	
	if (iDeathMsg || (iDeathMsg = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "DeathMsg", NULL)))
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

void CPugUtil::ScreenFade(edict_t *pEntity, float Duration, float HoldTime, int FadeFlags, int Red, int Green, int Blue, int Alpha)
{
	if (!FNullEnt(pEntity))
	{
		static int iMsgScreenFade;

		if (iMsgScreenFade || (iMsgScreenFade = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ScreenFade", nullptr)))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgScreenFade, nullptr, pEntity);
			g_engfuncs.pfnWriteShort(gPugUtil.FixedUnsigned16(Duration, BIT(12)));
			g_engfuncs.pfnWriteShort(gPugUtil.FixedUnsigned16(HoldTime, BIT(12)));
			g_engfuncs.pfnWriteShort(FadeFlags);
			g_engfuncs.pfnWriteByte(Red);
			g_engfuncs.pfnWriteByte(Green);
			g_engfuncs.pfnWriteByte(Blue);
			g_engfuncs.pfnWriteByte(Alpha);
			g_engfuncs.pfnMessageEnd();
		}
	}
	else
	{
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			pEntity = INDEXENT(i);

			if (this->IsNetClient(pEntity))
			{
				this->ScreenFade(pEntity, Duration, HoldTime, FadeFlags, Red, Green, Blue, Alpha);
			}
		}
	}
}

void CPugUtil::ScreenShake(edict_t *pEntity, float Amplitude, float Duration, float Frequency)
{
	if (!FNullEnt(pEntity))
	{
		static int iMsgScreenShake;

		if (iMsgScreenShake || (iMsgScreenShake = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ScreenShake", nullptr)))
		{
			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgScreenShake, nullptr, pEntity);
			g_engfuncs.pfnWriteShort(gPugUtil.FixedUnsigned16(Amplitude, BIT(12)));
			g_engfuncs.pfnWriteShort(gPugUtil.FixedUnsigned16(Duration, BIT(12)));
			g_engfuncs.pfnWriteShort(gPugUtil.FixedUnsigned16(Frequency, BIT(8)));
			g_engfuncs.pfnMessageEnd();
		}
	}
	else
	{
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			pEntity = INDEXENT(i);

			if (this->IsNetClient(pEntity))
			{
				this->ScreenShake(pEntity, Amplitude, Duration, Frequency);
			}
		}
	}
}

void CPugUtil::DrawTracer(Vector Start, Vector End)
{
	g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_TEMPENTITY, Start, nullptr);
	g_engfuncs.pfnWriteByte(TE_TRACER);
	g_engfuncs.pfnWriteCoord(Start.x);
	g_engfuncs.pfnWriteCoord(Start.y);
	g_engfuncs.pfnWriteCoord(Start.z);
	g_engfuncs.pfnWriteCoord(End.x);
	g_engfuncs.pfnWriteCoord(End.y);
	g_engfuncs.pfnWriteCoord(End.z);
	g_engfuncs.pfnMessageEnd();
}

void CPugUtil::DrawLine(Vector Start, Vector End, int Life, int Red, int Green, int Blue)
{
	g_engfuncs.pfnMessageBegin(MSG_BROADCAST, SVC_TEMPENTITY, Start, nullptr);
	g_engfuncs.pfnWriteByte(TE_LINE);
	g_engfuncs.pfnWriteCoord(Start.x);
	g_engfuncs.pfnWriteCoord(Start.y);
	g_engfuncs.pfnWriteCoord(Start.z);
	g_engfuncs.pfnWriteCoord(End.x);
	g_engfuncs.pfnWriteCoord(End.y);
	g_engfuncs.pfnWriteCoord(End.z);
	g_engfuncs.pfnWriteShort(Life);
	g_engfuncs.pfnWriteByte(Red);
	g_engfuncs.pfnWriteByte(Green);
	g_engfuncs.pfnWriteByte(Blue);
	g_engfuncs.pfnMessageEnd();
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

				if (Bots == false)
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

std::array<std::vector<CBasePlayer *>, SPECTATOR + 1> CPugUtil::GetPlayers()
{
	std::array<std::vector<CBasePlayer *>, SPECTATOR + 1> Players;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				Players[Player->m_iTeam].push_back(Player);
			}
		}
	}

	return Players;
}

void CPugUtil::ShowMotd(edict_t* pEntity, char* Motd, unsigned int MotdLength)
{
	static int iMsgMOTD;

	if (iMsgMOTD || (iMsgMOTD = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "MOTD", NULL)))
	{
		if (MotdLength < 128)
		{
			struct stat FileBuffer;

			if (stat(Motd, &FileBuffer) == 0)
			{
				int FileLength = 0;

				char* FileContent = reinterpret_cast<char*>(g_engfuncs.pfnLoadFileForMe(Motd, &FileLength));

				if (FileLength)
				{
					this->ShowMotd(pEntity, FileContent, FileLength);
				}

				g_engfuncs.pfnFreeFile(FileContent);

				return;
			}
		}

		char* Buffer = Motd;

		char Character = 0;

		int Size = 0;

		while (*Buffer)
		{
			Size = MotdLength;

			if (Size > 175)
			{
				Size = 175;
			}

			MotdLength -= Size;

			Character = *(Buffer += Size);

			*Buffer = 0;

			g_engfuncs.pfnMessageBegin(MSG_ONE, iMsgMOTD, NULL, pEntity);
			g_engfuncs.pfnWriteByte(Character ? FALSE : TRUE);
			g_engfuncs.pfnWriteString(Motd);
			g_engfuncs.pfnMessageEnd();

			*Buffer = Character;

			Motd = Buffer;
		}
	}
}

bool CPugUtil::IsPlayerVisible(CBasePlayer* Player, CBasePlayer* Target)
{
	if (Player && Target)
	{
		edict_t* pEntity = Player->edict();
		edict_t* pTarget = Target->edict();

		if (pEntity->v.flags & FL_NOTARGET)
		{
			return false;
		}
		
		Vector vLooker = (pEntity->v.origin + pEntity->v.view_ofs);
		Vector vTarget = (pTarget->v.origin + pTarget->v.view_ofs);

		TraceResult tr;

		auto oldSolid = pTarget->v.solid;

		pTarget->v.solid = SOLID_NOT;

		g_engfuncs.pfnTraceLine(vLooker, vTarget, FALSE, pEntity, &tr);

		pTarget->v.solid = oldSolid;

		if (tr.fInOpen && tr.fInWater)
		{
			return false;
		}
		else if ((tr.flFraction == 1.0) || (ENTINDEX(tr.pHit) == Target->entindex()))
		{
			return true;
		}
	}

	return false;
}