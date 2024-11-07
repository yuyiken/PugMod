#include "precompiled.h"

CPugPlayer gPugPlayer;

void CPugPlayer::ServerActivate()
{
	this->m_Players.clear();
}

const char* CPugPlayer::GetAuthId(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		if (!(pEntity->v.flags & FL_FAKECLIENT))
		{
			return g_engfuncs.pfnGetPlayerAuthId(pEntity);
		}

		return STRING(pEntity->v.netname);
	}

	return nullptr;
}

LP_PLAYER CPugPlayer::Get(const char* Auth)
{
	if (Auth)
	{
		if (Auth[0u] != '\0')
		{
			if (this->m_Players.find(Auth) != this->m_Players.end())
			{
				return &this->m_Players[Auth];
			}
		}
	}

	return nullptr;
}

LP_PLAYER CPugPlayer::Get(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		return this->Get(this->GetAuthId(pEntity));
	}

	return nullptr;
}

LP_PLAYER CPugPlayer::Get(int EntityIndex)
{
	return this->Get(INDEXENT(EntityIndex));
}

LP_PLAYER CPugPlayer::Get(CBasePlayer* Player)
{
	if (Player)
	{
		return this->Get(Player->entindex());
	}

	return nullptr;
}

void CPugPlayer::Connect(edict_t* pEntity, const char* pszName, const char* pszAddress)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = this->GetAuthId(pEntity);

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
				this->m_Players[Auth].TeamIndex = UNASSIGNED;
			}
		}
	}
}

void CPugPlayer::PutInServer(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Auth = this->GetAuthId(pEntity);

		if (Auth)
		{
			if (Auth[0u] != '\0')
			{
				this->m_Players[Auth].Name = STRING(pEntity->v.netname);
				this->m_Players[Auth].Auth = Auth;

				this->m_Players[Auth].ConnectTime = time(0);
				this->m_Players[Auth].Status = 2;
				this->m_Players[Auth].TeamIndex = UNASSIGNED;
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
			auto Auth = this->GetAuthId(pEntity);

			if (Auth)
			{
				if (Auth[0u] != '\0')
				{
					this->m_Players[Auth].Name = STRING(pEntity->v.netname);
					this->m_Players[Auth].Status = 3;
					this->m_Players[Auth].TeamIndex = Player->m_iTeam;

					this->m_Players[Auth].DeathMatch.Reset();
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
			auto Auth = this->GetAuthId(pEntity);

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

void CPugPlayer::SwitchTeam(CBasePlayer* Player)
{
	if (Player)
	{
		auto pEntity = Player->edict();

		if (!FNullEnt(pEntity))
		{
			auto Auth = this->GetAuthId(pEntity);

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