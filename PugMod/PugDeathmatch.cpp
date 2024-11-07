#include "precompiled.h"

CPugDeathmatch gPugDeathmatch;

void CPugDeathmatch::ServerActivate()
{
    this->m_Running = false;

	this->m_Spawns.clear();

	this->m_Items.clear();

	this->LoadSpawns();

	this->LoadItems();
}

void CPugDeathmatch::ServerDeactivate()
{
	this->m_Running = false;

	this->m_Spawns.clear();

	this->m_Items.clear();
}

void CPugDeathmatch::LoadSpawns()
{
	this->m_Spawns.clear();

	char Path[MAX_PATH] = { 0 };

	g_engfuncs.pfnGetGameDir(Path);

	Q_sprintf(Path, "%s/addons/pugmod/cfg/spawns/%s.cfg", Path, STRING(gpGlobals->mapname));

	std::ifstream File(Path);

	if (File)
	{
		auto LineCount = 1;

		std::string Line;

		P_DM_SPAWN Info;

		while (std::getline(File, Line))
		{
			std::istringstream Row(Line);

			if (Row >> Info.Vecs[0] >> Info.Vecs[1] >> Info.Vecs[2] >> Info.Angles[0] >> Info.Angles[1] >> Info.Angles[2] >> Info.Team >> Info.VAngles[0] >> Info.VAngles[1] >> Info.VAngles[2])
			{
				this->m_Spawns.push_back(Info);
			}
			else
			{
				gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Linha %d do arquivo %s incorreta.", Plugin_info.logtag, LineCount, STRING(gpGlobals->mapname));
			}

			LineCount++;
		}

		File.close();
	}
	else
	{
		gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Falha ao abrir: %s", Plugin_info.logtag, Path);
	}
}

void CPugDeathmatch::LoadItems()
{
	this->m_Items.clear();

	char Path[MAX_PATH] = { 0 };

	g_engfuncs.pfnGetGameDir(Path);

	Q_sprintf(Path, "%s/addons/pugmod/cfg/weapons.cfg", Path);

	std::ifstream File(Path);

	if (File)
	{
		auto LineCount = 1;

		std::string Line;

		P_DM_ITEM Info;

		while (std::getline(File, Line))
		{
			std::istringstream Row(Line);
			
			if (Row >> std::quoted(Info.Alias) >> std::quoted(Info.Label) >> Info.Enable >> Info.Bot >> Info.Slot)
			{
				Info.Alias = "weapon_" + Info.Alias;

				this->m_Items.push_back(Info);
			}
			else
			{
				gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Linha %d do arquivo %s incorreta.", Plugin_info.logtag, LineCount, STRING(gpGlobals->mapname));
			}

			LineCount++;
		}

		File.close();
	}
	else
	{
		gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Falha ao abrir: %s", Plugin_info.logtag, Path);
	}
}

void CPugDeathmatch::Init()
{
	this->m_Running = true;

	gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 O Modo ^3Deathmatch^1 está ativo até o início da partida.", Plugin_info.logtag);
}

void CPugDeathmatch::Stop()
{
	this->m_Running = false;

	gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 Modo ^3Deathmatch^1 desativado.", Plugin_info.logtag);
}

bool CPugDeathmatch::CheckDistance(CBasePlayer* Player, vec3_t Origin, float Distance)
{
	if (this->m_Running)
	{
		edict_t* pEntity = nullptr;

		while (!FNullEnt(pEntity = g_engfuncs.pfnFindEntityInSphere(pEntity, Origin, Distance)))
		{
			auto Target = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

			if (Target)
			{
				if (Target->IsPlayer() || Target->IsBot())
				{
					if (Target->entindex() != Player->entindex())
					{
						if (Target->IsAlive())
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}

bool CPugDeathmatch::SetPosition(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		if (this->m_Spawns.size())
		{
			do
			{
				auto Spawn = std::next(std::begin(this->m_Spawns), (std::rand() % this->m_Spawns.size()));

				if (!Spawn->Vecs.IsZero())
				{
					if (this->CheckDistance(Player, Spawn->Vecs, SPAWN_POINT_MIN_DISTANCE))
					{
						Player->edict()->v.origin = Spawn->Vecs + Vector(0.0f, 0.0f, 1.0f);

						Player->edict()->v.angles = Spawn->Angles;

						Player->edict()->v.v_angle = Spawn->VAngles;

						Player->edict()->v.v_angle.z = 0;

						Player->edict()->v.angles = Player->edict()->v.v_angle;

						Player->edict()->v.velocity = Vector(0.0f, 0.0f, 0.0f);

						Player->edict()->v.punchangle = Vector(0.0f, 0.0f, 0.0f);

						Player->edict()->v.fixangle = 1;

						Player->m_bloodColor = BLOOD_COLOR_RED;

						Player->m_modelIndexPlayer = Player->edict()->v.modelindex;

						if (Player->edict()->v.flags & FL_DUCKING)
						{
							g_engfuncs.pfnSetSize(Player->edict(), VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
						}
						else
						{
							g_engfuncs.pfnSetSize(Player->edict(), VEC_HULL_MIN, VEC_HULL_MAX);
						}

						return true;
					}
				}
			} while (true);
		}
	}

	return false;
}

bool CPugDeathmatch::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange)
{
	if (this->m_Running)
	{
		if (Type == RT_PLAYER_BOUGHT_SOMETHING)
		{
			return true;
		}
	}

	return false;
}

bool CPugDeathmatch::HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType)
{
	if (this->m_Running)
	{
		switch (ItemIndex)
		{
			case ITEM_SHIELDGUN:
			case ITEM_HEGRENADE:
			case ITEM_C4:
			case ITEM_SMOKEGRENADE:
			case ITEM_FLASHBANG:
			case ITEM_DEFUSEKIT:
			{
				gPugUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
				return true;
			}
		}
	}

	return false;
}

void CPugDeathmatch::PlayerSpawn(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		if (Player->IsBot())
		{
			this->EquipRandom(Player, 2);
			this->EquipRandom(Player, 1);
		}
		else
		{
			auto PugPlayer = gPugPlayer.Get(Player);

			if (PugPlayer)
			{
				if (PugPlayer->DeathMatch.EquipMenu)
				{
					this->EquipMenu(Player->entindex());
				}
				else
				{
					this->EquipLast(Player);
				}
			}
		}
	}
}

void CPugDeathmatch::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnimation)
{
	if (this->m_Running)
	{
		if (playerAnimation == PLAYER_ATTACK1 || playerAnimation == PLAYER_ATTACK2 || playerAnimation == PLAYER_RELOAD)
		{
			if (Player->m_pActiveItem)
			{
				if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
				{
					CBasePlayerWeapon* Weapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

					if (Weapon)
					{
						auto PugPlayer = gPugPlayer.Get(Player->entindex());

						if (PugPlayer)
						{
							PugPlayer->DeathMatch.State[Weapon->m_iId] = Weapon->m_iWeaponState;
						}
					}
				}
			}
		}
	}
}

bool CPugDeathmatch::SendDeathMessage(CBaseEntity* Killer, CBasePlayer* Victim, CBasePlayer* Assister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
	if (this->m_Running)
	{
		if (gPugCvar.m_DM_HideKillFeed)
		{
			if (gPugCvar.m_DM_HideKillFeed->value > 0.0f)
			{
				if (Killer)
				{
					if (Killer->IsPlayer())
					{
						auto Player = static_cast<CBasePlayer*>(Killer);

						if (Player)
						{
							auto PugPlayer = gPugPlayer.Get(Player);

							if (PugPlayer)
							{
								if (PugPlayer->DeathMatch.HideKillFeed > 0) // Disable this do not affect plugin 
								{
									gPugUtil.SendDeathMessage(Player->edict(), Player, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);

									if (Player->entindex() != Victim->entindex())
									{
										gPugUtil.SendDeathMessage(Victim->edict(), Player, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
									}

									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void CPugDeathmatch::GetIntoGame(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		if (!Player->IsBot())
		{
			gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Pressione ^3'G'^1 ou digite ^3'guns'^1 no chat para habilitar o menu de armas.", Plugin_info.logtag);
		}
	}
}

void CPugDeathmatch::EquipItem(CBasePlayer* Player, WeaponSlotInfo* Item)
{
	if (this->m_Running)
	{
		if (Player)
		{
			if (Item)
			{
				auto AutoWepSwitch = Player->m_iAutoWepSwitch;

				Player->m_iAutoWepSwitch = 1;

				auto PlayerItem = static_cast<CBasePlayerItem*>(Player->CSPlayer()->GiveNamedItemEx(Item->weaponName));

				if (PlayerItem)
				{
					Player->GiveAmmo(PlayerItem->CSPlayerItem()->m_ItemInfo.iMaxAmmo1, const_cast<char*>(PlayerItem->CSPlayerItem()->m_ItemInfo.pszAmmo1), -1);

					auto PugPlayer = gPugPlayer.Get(Player->entindex());

					if (PugPlayer)
					{
						PugPlayer->DeathMatch.Last[Item->slot] = Item;

						if (Player->m_pActiveItem)
						{
							if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
							{
								auto PlayerWeapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

								if (PlayerWeapon)
								{
									PlayerWeapon->m_iWeaponState = PugPlayer->DeathMatch.State[PlayerWeapon->m_iId];
								}
							}
						}
					}
				}

				Player->m_iAutoWepSwitch = AutoWepSwitch;
			}
		}
	}
}

void CPugDeathmatch::EquipRandom(CBasePlayer* Player, int Slot)
{
	if (this->m_Running)
	{
		if (Player)
		{
			if (this->m_Items.size() > 0)
			{
				do
				{
					auto Item = this->m_Items[(std::rand() % this->m_Items.size())];

					if (Item.Slot == Slot)
					{
						if (Player->IsBot() && (Item.Bot == 0))
						{
							continue;
						}

						auto SlotInfo = gReGameDLL.m_API->GetWeaponSlot(Item.Alias.c_str());

						if (SlotInfo)
						{
							this->EquipItem(Player, SlotInfo);
						}

						break;
					}
				}
				while (true);
			}
		}
	}
}

void CPugDeathmatch::EquipLast(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		auto PugPlayer = gPugPlayer.Get(Player);

		if (PugPlayer)
		{
			if (PugPlayer->DeathMatch.Last[2])
			{
				this->EquipItem(Player, PugPlayer->DeathMatch.Last[2]);
			}

			if (PugPlayer->DeathMatch.Last[1])
			{
				this->EquipItem(Player, PugPlayer->DeathMatch.Last[1]);
			}
		}
	}
}

bool CPugDeathmatch::SetEquipMenu(CBasePlayer* Player, bool ShowMenu)
{
	if (this->m_Running)
	{
		auto PugPlayer = gPugPlayer.Get(Player);

		if (PugPlayer)
		{
			if (ShowMenu)
			{
				gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 O menu de armas foi habilitado.", Plugin_info.logtag);
			}
			else
			{
				gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Pressione ^3'G'^1 ou digite ^3'guns'^1 no chat para habilitar o menu de armas.", Plugin_info.logtag);
			}

			PugPlayer->DeathMatch.EquipMenu = ShowMenu;

			return true;
		}
	}

	return false;
}

void CPugDeathmatch::EquipMenu(int EntityIndex)
{
	if (this->m_Running)
	{
		auto PugPlayer = gPugPlayer.Get(EntityIndex);

		if (PugPlayer)
		{
			gPugMenu[EntityIndex].Create("CSDM: Equipamentos", false, (void*)this->EquipMenuHandle);

			gPugMenu[EntityIndex].AddItem("1", "Novas Armas", false);

			if (PugPlayer->DeathMatch.Last[1] || PugPlayer->DeathMatch.Last[2])
			{
				gPugMenu[EntityIndex].AddItem("2", "Setup Anterior", false);
				gPugMenu[EntityIndex].AddItem("3", "2 + Ocultar Menu", false);
			}

			gPugMenu[EntityIndex].Show(EntityIndex);
		}
	}
}

void CPugDeathmatch::EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (std::stoi(Item.Info))
		{
			case 2:
			{
				gPugDeathmatch.EquipLast(Player);
				break;
			}
			case 3:
			{
				gPugDeathmatch.SetEquipMenu(Player, false);

				gPugDeathmatch.EquipLast(Player);
				break;
			}
			default:
			{
				gPugDeathmatch.WeaponMenu(Player->entindex(), 2);
				break;
			}
		}
	}
}

void CPugDeathmatch::WeaponMenu(int EntityIndex, int Slot)
{
	if (this->m_Running)
	{
		if (this->m_Items.size() > 0)
		{
			gPugMenu[EntityIndex].Create("CSDM: Armas", false, (void*)this->WeaponMenuHandle);

			for (auto const& Item : this->m_Items)
			{
				if (Item.Slot == Slot)
				{
					gPugMenu[EntityIndex].AddItem(Item.Alias, Item.Label, !Item.Enable, std::to_string(Item.Slot));
				}
			}

			gPugMenu[EntityIndex].Show(EntityIndex);
		}
	}
}

void CPugDeathmatch::WeaponMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			if (gReGameDLL.m_API)
			{
				auto SlotInfo = gReGameDLL.m_API->GetWeaponSlot(Item.Info.c_str());

				if (SlotInfo)
				{
					gPugDeathmatch.EquipItem(Player, SlotInfo);

					if (SlotInfo->slot == 2)
					{
						gPugDeathmatch.WeaponMenu(EntityIndex, 1);
					}
				}
			}
		}
	}
}

void CPugDeathmatch::OptionMenu(int EntityIndex)
{
	if (this->m_Running)
	{
		gPugMenu[EntityIndex].Create("CSDM: Opções", true, (void*)this->OptionMenuHandle);

		auto PugPlayer = gPugPlayer.Get(EntityIndex);

		if (PugPlayer)
		{
			gPugMenu[EntityIndex].AddItem("1", (PugPlayer->DeathMatch.HideKillFeed ? "Ocultar Kill Feed^R^yS" : "Ocultar Kill Feed^R^rN"));
			gPugMenu[EntityIndex].AddItem("2", (PugPlayer->DeathMatch.HitIndicator ? "Indicador de acerto^R^yS" : "Indicador de acerto^R^rN"));
			gPugMenu[EntityIndex].AddItem("3", (PugPlayer->DeathMatch.HSOnlyMode ? "Modo Headshot^R^yS" : "Modo Headshot^R^rN"));
			gPugMenu[EntityIndex].AddItem("4", (PugPlayer->DeathMatch.HudKDRatio ? "Exibir estatísticas^R^yS" : "Exibir estatísticas^R^rN"));
			gPugMenu[EntityIndex].AddItem("5", (PugPlayer->DeathMatch.KillFade ? "Piscar Tela ao matar^R^yS" : "Piscar Tela ao matar^R^rN"));
			gPugMenu[EntityIndex].AddItem("6", (PugPlayer->DeathMatch.KillSound ? "Som ao matar^R^yS" : "Som ao matar^R^rN"));
			gPugMenu[EntityIndex].AddItem("7", (PugPlayer->DeathMatch.MoneyFrag ? "Frags no dinheiro^R^yS" : "Frags no dinheiro^R^rN"));
		}

		gPugMenu[EntityIndex].Show(EntityIndex);
	}
}

void CPugDeathmatch::OptionMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto PugPlayer = gPugPlayer.Get(Player);

		if (PugPlayer)
		{
			switch (std::stoi(Item.Info))
			{
				case 1:
				{
					PugPlayer->DeathMatch.HideKillFeed ^= 1;
					break;
				}
				case 2:
				{
					PugPlayer->DeathMatch.HitIndicator ^= 1;
					break;
				}
				case 3:
				{
					PugPlayer->DeathMatch.HSOnlyMode ^= 1;
					break;
				}
				case 4:
				{
					PugPlayer->DeathMatch.HudKDRatio ^= 1;
					break;
				}
				case 5:
				{
					PugPlayer->DeathMatch.KillFade ^= 1;
					break;
				}
				case 6:
				{
					PugPlayer->DeathMatch.KillSound ^= 1;
					break;
				}
				case 7:
				{
					PugPlayer->DeathMatch.MoneyFrag ^= 1;
					break;
				}
			}

			gPugDeathmatch.OptionMenu(EntityIndex);
		}
	}
}