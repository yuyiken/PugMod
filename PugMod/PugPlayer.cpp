#include "precompiled.h"

CPugPlayer gPugPlayer;

LP_PLAYER CPugPlayer::Get(const char* Auth)
{
	if (Auth)
	{
		if (Auth[0u] != '\0')
		{
			if (this->m_Players.find(Auth) != this->m_Players.end())
			{
				this->m_Players[Auth];
			}
		}
	}

	return nullptr;
}

LP_PLAYER CPugPlayer::Get(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		return this->Get(g_engfuncs.pfnGetPlayerAuthId(pEntity));
	}

	return nullptr;
}

LP_PLAYER CPugPlayer::Get(int EntityIndex)
{
	return this->Get(INDEXENT(EntityIndex));
}

void CPugPlayer::Connect(edict_t* pEntity, const char* pszName, const char* pszAddress)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEntity);

		if (Auth)
		{
			if (Auth[0u] != '\0')
			{
				this->m_Players[Auth].EntityIndex = ENTINDEX(pEntity);

				this->m_Players[Auth].UserIndex = g_engfuncs.pfnGetPlayerUserId(pEntity);

				this->m_Players[Auth].Entity = pEntity;

				this->m_Players[Auth].Name = pszName;
				this->m_Players[Auth].Address = pszAddress;
				this->m_Players[Auth].Auth = Auth;

				this->m_Players[Auth].ConnectTime = time(0);
				this->m_Players[Auth].Status = 1;
				this->m_Players[Auth].TeamIndex = 0;
			}
		}
	}
}

void CPugPlayer::PutInServer(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEntity);

		if (Auth)
		{
			if (Auth[0u] != '\0')
			{
				this->m_Players[Auth].Name = STRING(pEntity->v.netname);
				this->m_Players[Auth].Auth = Auth;

				this->m_Players[Auth].ConnectTime = time(0);
				this->m_Players[Auth].Status = 2;
				this->m_Players[Auth].TeamIndex = 0;
			}
		}
	}
}

void CPugPlayer::GetIntoGame(CBasePlayer* Player)
{
	if (Player)
	{
		auto pEntity = Player->edict();

		if (!FNullEnt(pEntity))
		{
			auto Auth = g_engfuncs.pfnGetPlayerAuthId(Player->edict());

			if (Auth)
			{
				if (Auth[0u] != '\0')
				{
					this->m_Players[Auth].Name = STRING(pEntity->v.netname);
					this->m_Players[Auth].Status = 3;
					this->m_Players[Auth].TeamIndex = Player->m_iTeam;
				}
			}
		}
	}
}

void CPugPlayer::JoinTeam(CBasePlayer* Player)
{
	if (Player)
	{
		auto pEntity = Player->edict();

		if (!FNullEnt(pEntity))
		{
			auto Auth = g_engfuncs.pfnGetPlayerAuthId(Player->edict());

			if (Auth)
			{
				if (Auth[0u] != '\0')
				{
					this->m_Players[Auth].Name = STRING(pEntity->v.netname);
					this->m_Players[Auth].Status = 3;
					this->m_Players[Auth].TeamIndex = Player->m_iTeam;
				}
			}
		}
	}
}