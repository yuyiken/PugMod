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

LP_PLAYER_INFO CPugPlayer::GetInfo(int UserIndex)
{
	if (!this->m_Player.empty())
	{
		for (auto const& Player : this->m_Player)
		{
			if (Player.second.UserId == UserIndex)
			{
				auto PlayerInfo = this->GetInfo(Player.first.c_str());

				if (PlayerInfo)
				{
					return PlayerInfo;
				}
			}
		}
	}
	
	return nullptr;
}

bool CPugPlayer::Connect(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128])
{
	if (!FNullEnt(pEntity))
	{
		auto AuthId = this->GetPlayerAuthId(pEntity);

		if (AuthId)
		{
			if (!((pEntity->v.flags & FL_PROXY) == FL_PROXY))
			{
				this->m_Player[AuthId].EntityIndex = ENTINDEX(pEntity);

				this->m_Player[AuthId].UserId = g_engfuncs.pfnGetPlayerUserId(pEntity);

				this->m_Player[AuthId].Auth = AuthId;

				this->m_Player[AuthId].Name = (pszName != nullptr) ? pszName : "";

				this->m_Player[AuthId].Address = (pszAddress != nullptr) ? pszAddress : "";

				this->m_Player[AuthId].Flags = gPugAdmin.GetFlags(pEntity);

				this->m_Player[AuthId].AdminFlags = gPugAdmin.GetFlags(ENTINDEX(pEntity));

				this->m_Player[AuthId].Status = 1;

				this->m_Player[AuthId].DcReason.clear();

				this->m_Player[AuthId].LastTeam = TeamName::UNASSIGNED;

				this->m_Player[AuthId].Frags.fill(0);

				this->m_Player[AuthId].Deaths.fill(0);

				this->m_Player[AuthId].Money = 0;
			}
		}
	}

	return true;
}

void CPugPlayer::GetIntoGame(CBasePlayer* Player)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].EntityIndex = ENTINDEX(Player->edict());

		this->m_Player[AuthId].UserId = g_engfuncs.pfnGetPlayerUserId(Player->edict());

		this->m_Player[AuthId].Auth = AuthId;

		this->m_Player[AuthId].Name = STRING(Player->edict()->v.netname);

		this->m_Player[AuthId].Address = Player->IsBot() ? "loopback" : this->m_Player[AuthId].Address;

		this->m_Player[AuthId].Flags = gPugAdmin.GetFlags(Player->edict());

		this->m_Player[AuthId].AdminFlags = gPugAdmin.GetFlags(Player->entindex());

		this->m_Player[AuthId].Status = 2;

		this->m_Player[AuthId].DcReason.clear();

		this->m_Player[AuthId].LastTeam = Player->m_iTeam;

		this->m_Player[AuthId].Frags.fill(0);

		this->m_Player[AuthId].Deaths.fill(0);

		this->m_Player[AuthId].Money = 0;
	}
}

void CPugPlayer::SwitchTeam(CBasePlayer* Player)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].EntityIndex = ENTINDEX(Player->edict());

		this->m_Player[AuthId].UserId = g_engfuncs.pfnGetPlayerUserId(Player->edict());

		this->m_Player[AuthId].Auth = AuthId;

		this->m_Player[AuthId].Name = STRING(Player->edict()->v.netname);

		this->m_Player[AuthId].Address = Player->IsBot() ? "loopback" : this->m_Player[AuthId].Address;

		this->m_Player[AuthId].Flags = gPugAdmin.GetFlags(Player->edict());

		this->m_Player[AuthId].AdminFlags = gPugAdmin.GetFlags(Player->entindex());

		this->m_Player[AuthId].Status = 2;

		this->m_Player[AuthId].DcReason.clear();

		this->m_Player[AuthId].LastTeam = Player->m_iTeam;

		this->m_Player[AuthId].Frags[gPugMod.GetState()] = static_cast<int>(Player->edict()->v.frags);

		this->m_Player[AuthId].Deaths[gPugMod.GetState()] = Player->m_iDeaths;

		this->m_Player[AuthId].Money = Player->m_iAccount;
	}
}

void CPugPlayer::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange)
{
	auto AuthId = this->GetPlayerAuthId(Player->edict());

	if (AuthId)
	{
		this->m_Player[AuthId].Frags[gPugMod.GetState()] = static_cast<int>(Player->edict()->v.frags);

		this->m_Player[AuthId].Deaths[gPugMod.GetState()] = Player->m_iDeaths;

		this->m_Player[AuthId].Money = Player->m_iAccount;
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

// void CPugPlayer::PlayerMenu(CBasePlayer* Player)
// {
// 	auto EntityIndex = Player->entindex();

// 	if (EntityIndex)
// 	{
// 		auto Flags = gPugAdmin.GetFlags(EntityIndex);

// 		if (!(Flags & ADMIN_MENU))
// 		{
// 			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
// 			return;
// 		}

// 		if (!this->m_Player.empty())
// 		{
// 			gMatchMenu[EntityIndex].Create(_T("Player List"), true, (void*)this->PlayerMenuHandle);

// 			for (auto const& Target : this->m_Player)
// 			{
// 				if (!gPugAdmin.Access(Target.second.Auth, ADMIN_IMMUNITY))
// 				{
// 					if (Target.second.Status == 2)
// 					{
// 						gMatchMenu[EntityIndex].AddItem(Target.second.UserId, gMatchUtil.FormatString("%s \\R\\y%s", Target.second.Name.c_str(), gMatchBot.GetTeam(Target.second.LastTeam, true)), false, Target.second.UserId);
// 					}
// 					else
// 					{
// 						gMatchMenu[EntityIndex].AddItem(Target.second.UserId, gMatchUtil.FormatString("%s \\R\\y%s", Target.second.Name.c_str(), (Target.second.Status == 1) ? _T("Online") : _T("Offline")), false, Target.second.UserId);
// 					}
// 				}
// 			}

// 			gMatchMenu[EntityIndex].Show(EntityIndex);
// 		}
// 		else
// 		{
// 			gMatchUtil.SayText(INDEXENT(EntityIndex), E_PRINT_TEAM::DEFAULT, _T("Player List is empty..."));
// 		}
// 	}
// }

// void CPugPlayer::PlayerMenuHandle(int EntityIndex, P_MENU_ITEM Item)
// {
// 	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

// 	if (Player)
// 	{
// 		gPugPlayer.ShowInfo(Player, Item.Info);
// 	}
// }

// void CPugPlayer::ShowInfo(CBasePlayer* Player, int UserIndex)
// {
// 	auto PlayerInfo = this->GetInfo(UserIndex);

// 	if (PlayerInfo)
// 	{
// 		auto EntityIndex = Player->entindex();

// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s", std::string(32, '=').c_str());
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "Nome", PlayerInfo->Name.c_str());
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "Steam", PlayerInfo->Auth.c_str());
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "IP", PlayerInfo->Address.c_str());
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "Flags", PlayerInfo->Flags.c_str());
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "Time", g_Pug_TeamId[PlayerInfo->LastTeam]);
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: #%d", "User Index", PlayerInfo->UserId);
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s: %s", "Status", (PlayerInfo->Status > 0) ? (PlayerInfo->Status == 1 ? "Conectado" : "No Jogo") : "Desconectado");
// 		gPugUtil.ClientPrint(Player->edict(), E_PRINT::CONSOLE, "%s", std::string(32, '=').c_str());

// 		gPugMenu[EntityIndex].Create
// 		(
// 			gPugUtil.FormatString
// 			(
// 				"%s:\\w\n\n%s: \\y%s\\w\n%s: \\y%s\\w\n%s: \\y%s\\w\n%s: \\y%s\\w\n%s: \\y#%d\\w\n%s: \\y%s",
// 				PlayerInfo->Name.c_str(),
// 				"Steam",
// 				PlayerInfo->Auth.c_str(),
// 				"IP",
// 				PlayerInfo->Address.c_str(),
// 				"Flags",
// 				PlayerInfo->Flags.c_str(),
// 				"Time",
// 				g_Pug_TeamId[PlayerInfo->LastTeam],
// 				"User Index",
// 				PlayerInfo->UserId,
// 				"Status",
// 				(PlayerInfo->Status > 0) ? (PlayerInfo->Status == 1 ? "Conectado" : "No Jogo") : "Desconectado"
// 			),
// 			true,
// 			E_MENU::ME_ADMIN_MENU_PLAYER
// 		);

// 		auto IsImmunity = static_cast<bool>(PlayerInfo->AdminFlags & ADMIN_IMMUNITY);

// 		auto IsConnected = (PlayerInfo->Status > 0);

// 		gPugMenu[EntityIndex].AddItem(0, "Banir", IsImmunity, PlayerInfo->UserId);

// 		gPugMenu[EntityIndex].AddItem(1, "Kickar", (IsImmunity || !IsConnected), PlayerInfo->UserId);

// 		gPugMenu[EntityIndex].AddItem(2, "Matar", (IsImmunity || !IsConnected), PlayerInfo->UserId);

// 		gPugMenu[EntityIndex].AddItem(3, "Trocar Time", (IsImmunity || !IsConnected), PlayerInfo->UserId);

// 		gPugMenu[EntityIndex].Show(Player);
// 	}
// }

// void CPugPlayer::PlayerMenuActionHandle(CBasePlayer *Player, P_MENU_ITEM Item)
// {
// 	if (Player)
// 	{
//         auto EntityIndex = Player->entindex();

// 		if (Item.Extra)
// 		{
// 			auto PlayerInfo = gPugPlayer.GetInfo(Item.Extra);

// 			if (PlayerInfo)
// 			{
// 				switch (Item.Info)
// 				{
// 					case 0: // Ban
// 					{
// 						gPugMenu[EntityIndex].Create(gPugUtil.FormatString("Tempo para banir: ^w%s^y", PlayerInfo->Name.c_str()), true, E_MENU::ME_ADMIN_MENU_PLAYER_BAN);

// 						std::vector<time_t> BanTimes = { 0, 5, 10, 15, 30, 60, 120, 240, 480, 960, 1440, 10080, 43200 };

// 						for (auto const& Time : BanTimes)
// 						{
// 							gMatchMenu[EntityIndex].AddItem(Time, CTimeFormat::GetTimeLength(Time, CTimeFormat::TIMEUNIT_MINUTES), false, Item.Extra);
// 						}

// 						gMatchMenu[EntityIndex].Show(EntityIndex);

// 						break;
// 					}
// 					case 1: // Kick
// 					{
// 						auto Target = gMatchUtil.GetPlayerByUserId(Item.Extra);

// 						if (Target)
// 						{
// 							gMatchUtil.ServerCommand("kick #%d", Item.Extra);
// 						}
// 						else
// 						{
// 							gMatchUtil.SayText(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^3%s^1 is not in connected."), PlayerInfo->Name.c_str());
// 						}
						
// 						break;
// 					}
// 					case 2: // Kill
// 					{
// 						auto Target = gMatchUtil.GetPlayerByUserId(Item.Extra);

// 						if (Target)
// 						{
// 							MDLL_ClientKill(Target->edict());
// 						}
// 						else
// 						{
// 							gMatchUtil.SayText(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^3%s^1 is not in connected."), PlayerInfo->Name.c_str());
// 						}

// 						break;
// 					}
// 					case 3: // Change Team
// 					{
// 						auto Target = gMatchUtil.GetPlayerByUserId(Item.Extra);

// 						if (Target)
// 						{
// 							gPugAdminMenu.TeamMenuHandle(Player->entindex(), { Target->entindex(), STRING(Target->edict()->v.netname), false, 0 });
// 						}
// 						else
// 						{
// 							gMatchUtil.SayText(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^3%s^1 is not in connected."), PlayerInfo->Name.c_str());
// 						}

// 						break;
// 					}
// 				}
// 			}
// 		}
// 	}
// }

// void CPugPlayer::PlayerBanMenuActionHandle(CBasePlayer *Player, P_MENU_ITEM Item)
// {
// 	if (Player)
// 	{
// 		auto Info = gPugPlayer.GetInfo(Item.Extra);

// 		if (Info)
// 		{
// 			gPugUtil.ServerCommand("banid %d %s kick", Item.Info, Info->Auth.c_str());

// 			gPugUtil.ServerCommand("wait;wait;writeid;writeip");
// 		}
// 	}
// }