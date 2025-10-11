#include "precompiled.h"

CPugPlayer gPugPlayer;

void CPugPlayer::ServerActivate()
{
	this->m_Player.clear();
}

const char* CPugPlayer::GetPlayerAuthId(edict_t* pEntity)
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

LP_PLAYER_INFO CPugPlayer::GetInfo(const char* AuthId)
{
	if (AuthId)
	{
		if (AuthId[0u] != '\0')
		{
			if (this->m_Player.find(AuthId) != this->m_Player.end())
			{
				return &this->m_Player[AuthId];
			}
		}
	}

	return nullptr;
}

LP_PLAYER_INFO CPugPlayer::GetInfo(int EntityIndex)
{
	if (!this->m_Player.empty())
	{
		for (auto const& Player : this->m_Player)
		{
			if (Player.second.EntityId == EntityIndex)
			{
				if (!Player.first.empty())
				{
					return this->GetInfo(Player.first.c_str());
				}
			}
		}
	}
	
	return nullptr;
}

void CPugPlayer::PutInServer(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto AuthId = this->GetPlayerAuthId(pEntity);

		if (AuthId)
		{
			if (!((pEntity->v.flags & FL_PROXY) == FL_PROXY))
			{
				this->m_Player[AuthId].EntityId = ENTINDEX(pEntity);

				this->m_Player[AuthId].UserId = g_engfuncs.pfnGetPlayerUserId(pEntity);

				this->m_Player[AuthId].Auth = AuthId;

				this->m_Player[AuthId].Name = STRING(pEntity->v.netname);

				this->m_Player[AuthId].Flags = gPugAdmin.GetFlags(ENTINDEX(pEntity));

				this->m_Player[AuthId].Status = 1;

				this->m_Player[AuthId].DcReason.clear();

				this->m_Player[AuthId].Team = TeamName::UNASSIGNED;

				this->m_Player[AuthId].Stats.fill({});
			}
		}
	}
}

void CPugPlayer::GetIntoGame(CBasePlayer* Player)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].Name = STRING(Player->edict()->v.netname);

		this->m_Player[AuthId].Status = 2;

		this->m_Player[AuthId].Team = Player->m_iTeam;

		auto State = gPugMod.GetState();

		if (State >= STATE_DEAD && State <= STATE_END)
		{
			this->m_Player[AuthId].Stats[State].Frags = Player->edict()->v.frags;

			this->m_Player[AuthId].Stats[State].Deaths = Player->m_iDeaths;

			this->m_Player[AuthId].Stats[State].Account = Player->m_iAccount;
		}
	}
}

void CPugPlayer::SwitchTeam(CBasePlayer* Player)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].Name = STRING(Player->edict()->v.netname);

		this->m_Player[AuthId].Status = (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT) ? 2 : 1;

		this->m_Player[AuthId].Team = Player->m_iTeam;

		auto State = gPugMod.GetState();

		if (State >= STATE_DEAD && State <= STATE_END)
		{
			this->m_Player[AuthId].Stats[State].Frags = Player->edict()->v.frags;

			this->m_Player[AuthId].Stats[State].Deaths = Player->m_iDeaths;

			this->m_Player[AuthId].Stats[State].Account = Player->m_iAccount;
		}
	}
}

void CPugPlayer::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].Name = STRING(Player->edict()->v.netname);

		this->m_Player[AuthId].Status = (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT) ? 2 : 1;

		this->m_Player[AuthId].Team = Player->m_iTeam;

		auto State = gPugMod.GetState();

		if (State >= STATE_DEAD && State <= STATE_END)
		{
			this->m_Player[AuthId].Stats[State].Frags = Player->edict()->v.frags;

			this->m_Player[AuthId].Stats[State].Deaths = Player->m_iDeaths;

			this->m_Player[AuthId].Stats[State].Account = Player->m_iAccount;
		}
	}
}

void CPugPlayer::Disconnect(edict_t* pEntity, bool crash, const char* Reason)
{
	if (!FNullEnt(pEntity))
	{
		auto AuthId = this->GetPlayerAuthId(pEntity);

		if (AuthId)
		{
			if (!((pEntity->v.flags & FL_PROXY) == FL_PROXY))
			{
				this->m_Player[AuthId].Name = STRING(pEntity->v.netname);

				this->m_Player[AuthId].Status = 0;

				if (Reason)
				{
					if (Reason[0u] != '\0')
					{
						this->m_Player[AuthId].DcReason = Reason;
					}
				}
			}
		}
	}
}
