#include "precompiled.h"

CPugDeathmatch gPugDeathmatch;

void CPugDeathmatch::ServerActivate()
{
    this->m_Running = false;

    this->LoadSpawns();

	this->LoadWeapons();
}

void CPugDeathmatch::LoadSpawns()
{
    this->m_Spawns.clear();

    char Path[MAX_PATH] = { 0 };

    Q_sprintf(Path, "%s/csdm/spawns/%s.txt", gPugConfig.GetFullPath().c_str(), STRING(gpGlobals->mapname));

    std::ifstream File(Path);

    if (File)
    {
        auto LineCount = 1;

        std::string Line = "";

        P_SPAWN_POINT Info;

        while (std::getline(File, Line))
        {
            std::istringstream Row(Line);

            if (!(Row >> Info.Vecs[0] >> Info.Vecs[1] >> Info.Vecs[2] >> Info.Angles[0] >> Info.Angles[1] >> Info.Angles[2] >> Info.Team >> Info.VAngles[0] >> Info.VAngles[1] >> Info.VAngles[2]))
            {
                gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Line %d of the %s spawns file is incorrect.", Plugin_info.logtag, LineCount, STRING(gpGlobals->mapname));
            }

            this->m_Spawns.insert(std::make_pair(this->m_Spawns.size(), Info));

            LineCount++;
        }

        File.close();
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Failed to read file: %s", __func__, Path);
    }
}

void CPugDeathmatch::LoadWeapons()
{
    this->m_Weapons.clear();

    char Path[MAX_PATH] = { 0 };

    Q_sprintf(Path, "%s/csdm/weapons.txt", gPugConfig.GetFullPath().c_str());

    std::ifstream File(Path);

    if (File)
    {
        std::string Line;

        P_ITEM_INFO Info;

        while (std::getline(File, Line))
        {
            std::istringstream Row(Line);

            if (Row >> Info.Alias >> Info.Label >> Info.Enabled >> Info.Bot >> Info.Slot)
            {
                Info.Alias.erase(std::remove(Info.Alias.begin(), Info.Alias.end(), '\"'), Info.Alias.end());

                Info.Alias = "weapon_" + Info.Alias;

                Info.Label.erase(std::remove(Info.Label.begin(), Info.Label.end(), '\"'), Info.Label.end());

                this->m_Weapons.push_back(Info);
            }
        }

        File.close();
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(&Plugin_info, "[%s] Failed to read file: %s", __func__, Path);
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

bool CPugDeathmatch::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange)
{
	if (this->m_Running)
	{
		if (this->m_Running && Type == RT_PLAYER_BOUGHT_SOMETHING)
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

bool CPugDeathmatch::SetPlayerPosition(CBasePlayer* Player)
{
    if (this->m_Running)
    {
        if (this->m_Spawns.size())
        {
            do
            {
                auto Spawn = std::next(std::begin(this->m_Spawns), (std::rand() % this->m_Spawns.size()));

                if (!Spawn->second.Vecs.IsZero())
                {
                    if (this->CheckDistance(Player, Spawn->second.Vecs, SPAWN_POINT_MIN_DISTANCE))
                    {
                        Player->edict()->v.origin = Spawn->second.Vecs + Vector(0.0f, 0.0f, 1.0f);

                        Player->edict()->v.angles = Spawn->second.Angles;

                        Player->edict()->v.v_angle = Spawn->second.VAngles;

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

void CPugDeathmatch::PlayerSpawn(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		if (!Player->IsBot())
		{
			if (!this->GetHideMenu(Player->entindex()))
			{
				this->EquipMenu(Player->entindex());
			}
			else
			{
				this->EquipLast(Player);
			}
		}
		else
		{
			this->EquipRandom(Player, 2);

			this->EquipRandom(Player, 1);
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
				if (Player->m_pActiveItem->m_iId == WEAPON_USP || Player->m_pActiveItem->m_iId == WEAPON_GLOCK18 || Player->m_pActiveItem->m_iId == WEAPON_FAMAS || Player->m_pActiveItem->m_iId == WEAPON_M4A1)
				{
					CBasePlayerWeapon* Weapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

					if (Weapon)
					{
						this->SetWeaponState(Player->entindex(), Weapon);
					}
				}
			}
		}
	}
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
				if (Target->IsPlayer())
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

void CPugDeathmatch::ResetPlayer(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		this->m_Info[Player->entindex()].Reset();

		if (!Player->IsBot())
		{
			gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Pressione ^3'G'^1 ou digite ^3'guns'^1 no chat para habilitar o menu de armas.", Plugin_info.logtag);
		}
	}
}

void CPugDeathmatch::EquipItem(CBasePlayer* Player, InventorySlotType SlotType, std::string Item)
{
	if (this->m_Running)
	{
		if (Player)
		{
			auto AutoWepSwitch = Player->m_iAutoWepSwitch;

			Player->m_iAutoWepSwitch = 1;

			auto PlayerItem = static_cast<CBasePlayerItem*>(Player->CSPlayer()->GiveNamedItemEx(Item.c_str()));

			if (PlayerItem)
			{
				Player->GiveAmmo(PlayerItem->CSPlayerItem()->m_ItemInfo.iMaxAmmo1, const_cast<char*>(PlayerItem->CSPlayerItem()->m_ItemInfo.pszAmmo1), -1);

				if (SlotType == InventorySlotType::PISTOL_SLOT)
				{
					this->m_Info[Player->entindex()].LastSecondary = Item;
				}
				else if (SlotType == InventorySlotType::PRIMARY_WEAPON_SLOT)
				{
					this->m_Info[Player->entindex()].LastPrimary = Item;
				}
			}

			if (Player->m_pActiveItem)
			{
				if (Player->m_pActiveItem->m_iId == WEAPON_USP || Player->m_pActiveItem->m_iId == WEAPON_GLOCK18 || Player->m_pActiveItem->m_iId == WEAPON_FAMAS || Player->m_pActiveItem->m_iId == WEAPON_M4A1)
				{
					auto Weapon = static_cast<CBasePlayerWeapon*>(Player->m_pActiveItem);

					if (Weapon)
					{
						Weapon->m_iWeaponState = this->m_Info[Player->entindex()].WeaponState[Weapon->m_iId];
					}
				}
			}

			Player->m_iAutoWepSwitch = AutoWepSwitch;
		}
	}
}

void CPugDeathmatch::EquipRandom(CBasePlayer* Player, int Slot)
{
	if (this->m_Running)
	{
		if (Player)
		{
			if (this->m_Weapons.size())
			{
				do
				{
					auto Item = std::next(std::begin(this->m_Weapons), (std::rand() % this->m_Weapons.size()));

					if (Item->Enabled)
					{
						if (Item->Slot == Slot)
						{
							if (!Item->Bot && Player->IsBot())
							{
								continue;
							}

							this->EquipItem(Player, InventorySlotType::PISTOL_SLOT, Item->Alias);

							break;
						}
					}
				}
				while (true);
			}
		}
	}
}

bool CPugDeathmatch::EquipLast(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		auto Result = false;

		if (!this->m_Info[Player->entindex()].LastSecondary.empty())
		{
			Result = true;

			this->EquipItem(Player, InventorySlotType::PISTOL_SLOT, this->m_Info[Player->entindex()].LastSecondary);
		}

		if (!this->m_Info[Player->entindex()].LastPrimary.empty())
		{
			Result = true;

			this->EquipItem(Player, InventorySlotType::PRIMARY_WEAPON_SLOT, this->m_Info[Player->entindex()].LastPrimary);
		}

		return Result;
	}

	return false;
}

bool CPugDeathmatch::SetHideMenu(CBasePlayer* Player, bool HideMenu)
{
	if (this->m_Running)
	{
		if (Player)
		{
			auto EntityIndex = Player->entindex();

			if (HideMenu)
			{
				if (!this->m_Info[EntityIndex].HideMenu)
				{
					gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Pressione ^3'G'^1 ou digite ^3'guns'^1 no chat para habilitar o menu de armas.", Plugin_info.logtag);
				}
			}
			else
			{
				if (this->m_Info[EntityIndex].HideMenu)
				{
					gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 O menu de armas foi habilitado.", Plugin_info.logtag);
				}
			}

			this->m_Info[EntityIndex].HideMenu = HideMenu;

			return true;
		}
	}

	return false;
}

bool CPugDeathmatch::GetHideMenu(int EntityIndex)
{
	return this->m_Info[EntityIndex].HideMenu;
}

void CPugDeathmatch::SetWeaponState(int EntityIndex, CBasePlayerWeapon* Weapon)
{
	if (Weapon)
	{
		this->m_Info[EntityIndex].WeaponState[Weapon->m_iId] = Weapon->m_iWeaponState;
	}
}

int CPugDeathmatch::GetWeaponState(int EntityIndex, int WeaponIndex)
{
	return this->m_Info[EntityIndex].WeaponState[WeaponIndex];
}

void CPugDeathmatch::EquipMenu(int EntityIndex)
{
	if (this->m_Running)
	{
		gPugMenu[EntityIndex].Create("CSDM: Equipamentos", false, (void*)this->EquipMenuHandle);

		gPugMenu[EntityIndex].AddItem("1", "Novas Armas", false);

		if (!this->m_Info[EntityIndex].LastSecondary.empty() || !this->m_Info[EntityIndex].LastPrimary.empty())
		{
			gPugMenu[EntityIndex].AddItem("2", "Setup Anterior", false);
			gPugMenu[EntityIndex].AddItem("3", "2 + E ocultar o menu", false);
		}

		gPugMenu[EntityIndex].Show(EntityIndex);
	}
}

void CPugDeathmatch::EquipMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			switch (std::stoi(Item.Info))
			{
				case 1:
				{
					gPugDeathmatch.SecondaryMenu(EntityIndex);
					return;
				}
				case 2:
				{
					if (gPugDeathmatch.EquipLast(Player))
					{
						return;
					}
					break;
				}
				case 3:
				{
					gPugDeathmatch.SetHideMenu(Player, true);

					if (gPugDeathmatch.EquipLast(Player))
					{
						return;
					}
					break;
				}
			}
		}

		gPugDeathmatch.EquipMenu(Player->entindex());
	}
}

void CPugDeathmatch::SecondaryMenu(int EntityIndex)
{
	if (this->m_Running)
	{
		gPugMenu[EntityIndex].Create("CSDM: Armas Secundárias", false, (void*)this->SecondaryMenuHandle);

		for (auto const& Weapon : this->m_Weapons)
		{
			if (Weapon.Slot == 2)
			{
				gPugMenu[EntityIndex].AddItem(Weapon.Alias, Weapon.Label, Weapon.Enabled ? false : true);
			}
		}

		gPugMenu[EntityIndex].Show(EntityIndex);
	}
}

void CPugDeathmatch::SecondaryMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			gPugDeathmatch.EquipItem(Player, InventorySlotType::PISTOL_SLOT, Item.Info);

			gPugDeathmatch.PrimaryMenu(EntityIndex);
		}
	}
}

void CPugDeathmatch::PrimaryMenu(int EntityIndex)
{
	if (this->m_Running)
	{
		gPugMenu[EntityIndex].Create("CSDM: Primary Weapons", false, (void*)this->PrimaryMenuHandle);

		for (auto const& Weapon : this->m_Weapons)
		{
			if (Weapon.Slot == 1)
			{
				gPugMenu[EntityIndex].AddItem(Weapon.Alias, Weapon.Label, Weapon.Enabled ? false : true);
			}
		}

		gPugMenu[EntityIndex].Show(EntityIndex);
	}
}

void CPugDeathmatch::PrimaryMenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (!Item.Disabled)
		{
			gPugDeathmatch.EquipItem(Player, InventorySlotType::PRIMARY_WEAPON_SLOT, Item.Info);
		}
		else
		{
			gPugDeathmatch.PrimaryMenu(EntityIndex);
		}
	}
}