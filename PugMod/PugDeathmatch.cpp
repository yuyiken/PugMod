#include "precompiled.h"

CPugDeathmatch gPugDeathmatch;

void CPugDeathmatch::ServerActivate()
{
    this->m_Spawns.clear();
    this->m_Weapon.clear();

    char Path[MAX_PATH] = {0};

    g_engfuncs.pfnGetGameDir(Path);

    sprintf(Path, "%s/addons/pugmod/spawns/%s.txt", Path, STRING(gpGlobals->mapname));

    std::ifstream File(Path);

    if (File)
    {
        auto LineCount = 1;

        std::string Line = "";

        P_SPAWN Spawn = {};

        while (std::getline(File, Line))
        {
            std::istringstream Row(Line);

            Spawn = {};

            if (!(Row >> Spawn.Vecs[0] >> Spawn.Vecs[1] >> Spawn.Vecs[2] >> Spawn.Angles[0] >> Spawn.Angles[1] >> Spawn.Angles[2] >> Spawn.Team >> Spawn.VAngles[0] >> Spawn.VAngles[1] >> Spawn.VAngles[2]))
            {
                gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Linha %d do arquivo %s incompleta.", Plugin_info.logtag, LineCount, STRING(gpGlobals->mapname));
            }

            if (!Spawn.Vecs.IsZero())
            {
                this->m_Spawns.push_back(Spawn);
            }

            LineCount++;
        }

        File.close();
    }
    else
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Falha ao abrir: %s", Plugin_info.logtag, Path);
    }
}

bool CPugDeathmatch::GetPlayerSpawnSpot(CBasePlayer *Player)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (this->m_Spawns.size())
        {
            do
            {
                auto Spawn = this->m_Spawns.begin();

                std::advance(Spawn, g_engfuncs.pfnRandomLong(0, this->m_Spawns.size()));

                if (!Spawn->Vecs.IsZero())
                {
                    if (this->CheckDistance(Player, Spawn->Vecs, SPAWN_POINT_MIN_DISTANCE))
                    {
                        Player->edict()->v.origin = Spawn->Vecs + Vector(0.0f, 0.0f, 1.0f);

                        if (!Spawn->Angles.IsZero())
                        {
                            Player->edict()->v.angles = Spawn->Angles;
                        }

                        if (!Spawn->VAngles.IsZero())
                        {
                            Player->edict()->v.v_angle = Spawn->VAngles;

                            Player->edict()->v.v_angle.z = 0;

                            Player->edict()->v.angles = Player->edict()->v.v_angle;
                        }

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
            }
            while (true);
        }
    }

    return false;
}

bool CPugDeathmatch::CheckDistance(CBasePlayer *Player, vec3_t Origin, float Distance)
{
    edict_t *pEntity = nullptr;

    while (!FNullEnt(pEntity = g_engfuncs.pfnFindEntityInSphere(pEntity, Origin, Distance)))
    {
        auto Target = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

        if (Target)
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

    return true;
}

void CPugDeathmatch::AddWeapon(std::string Alias, std::string Label, bool Bot)
{
    if (g_ReGameApi)
    {
        if (!Alias.empty() && !Label.empty())
        {
            if (Alias.find("weapon_") == std::string::npos)
            {
                Alias = "weapon_" + Alias;
            }

            auto SlotInfo = g_ReGameApi->GetWeaponSlot(Alias.c_str());

            if (SlotInfo)
            {
                this->m_Weapon.push_back({SlotInfo, Label, Bot});
            }  
        }
    }
}

void CPugDeathmatch::MenuOption(int EntityIndex)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            auto PlayerInfo = gPugPlayer.GetInfo(EntityIndex);

            if (PlayerInfo)
            {
                gPugMenu[EntityIndex].Create("CSDM: Opções", true, (void *)this->MenuOptionHandle);

                gPugMenu[EntityIndex].AddItemFormat(0, false, 0, "Ocultar Kill Feed^R%s", PlayerInfo->m_DM.m_HideKillFeed ? "^yS" : "^rN");
                gPugMenu[EntityIndex].AddItemFormat(1, false, 0, "Indicador de dano^R%s", PlayerInfo->m_DM.m_HitIndicator ? "^yS" : "^rN");
                gPugMenu[EntityIndex].AddItemFormat(2, false, 0, "Modo headshot^R%s", PlayerInfo->m_DM.m_HSOnlyMode ? "^yS" : "^rN");
                gPugMenu[EntityIndex].AddItemFormat(3, false, 0, "Exibir estatísticas^R%s", PlayerInfo->m_DM.m_HudKDRatio ? "^yS" : "^rN");
                gPugMenu[EntityIndex].AddItemFormat(4, false, 0, "Piscar ao matar^R^y%d", PlayerInfo->m_DM.m_KillFade);
                gPugMenu[EntityIndex].AddItemFormat(5, false, 0, "Emitir som ao matar^R^y%d", PlayerInfo->m_DM.m_KillSound);
                gPugMenu[EntityIndex].AddItemFormat(6, false, 0, "Exibir frags no dinheiro^R%s", PlayerInfo->m_DM.m_MoneyFrag ? "^yS" : "^rN");
                gPugMenu[EntityIndex].AddItemFormat(7, (Player->edict()->v.frags < 1.0f), Player->m_iDeaths, "Resetar Score");

                gPugMenu[EntityIndex].Show(EntityIndex);
            }
        }
    }
}

void CPugDeathmatch::MenuOptionHandle(int EntityIndex, P_MENU_ITEM Item)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            auto PlayerInfo = gPugPlayer.GetInfo(EntityIndex);

            if (PlayerInfo)
            {
                switch (Item.Info)
                {
                    case 0:
                    {
                        PlayerInfo->m_DM.m_HideKillFeed ^= 1;
                        break;
                    }
                    case 1:
                    {
                        PlayerInfo->m_DM.m_HitIndicator ^= 1;
                        break;
                    }
                    case 2:
                    {
                        PlayerInfo->m_DM.m_HSOnlyMode ^= 1;
                        break;
                    }
                    case 3:
                    {
                        PlayerInfo->m_DM.m_HudKDRatio ^= 1;
                        break;
                    }
                    case 4:
                    {
                        PlayerInfo->m_DM.m_KillFade = (PlayerInfo->m_DM.m_KillFade < 2) ? (PlayerInfo->m_DM.m_KillFade + 1) : 0;
                        break;
                    }
                    case 5:
                    {
                        PlayerInfo->m_DM.m_KillSound = (PlayerInfo->m_DM.m_KillSound < 2) ? (PlayerInfo->m_DM.m_KillSound + 1) : 0;
                        break;
                    }
                    case 6:
                    {
                        PlayerInfo->m_DM.m_MoneyFrag ^= 1;
                        break;
                    }
                    case 7:
                    {
                        if (!Item.Disabled)
                        {
                            Player->CSPlayer()->Reset();

                            Player->RoundRespawn();

                            PlayerInfo->m_DM.m_Headshots = 0;

                            gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 ^3%s^1 resetou o score.", gPugCvar.m_Tag->string, STRING(Player->edict()->v.netname));
                        }
                        else
                        {
                            gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Não há score para resetar.", gPugCvar.m_Tag->string);
                        }
                        break;
                    }
                }

                gPugDeathmatch.MenuOption(EntityIndex);
            }
        }
    }
}

void CPugDeathmatch::Menu(CBasePlayer *Player)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto EntityIndex = Player->entindex();

        if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
        {
            if (gPugCvar.m_DM_KillRepairArmor->value > 0.0f)
            {
                Player->edict()->v.armorvalue = MAX_NORMAL_BATTERY;
                Player->m_iKevlar = ARMOR_VESTHELM;
            }

            if (!Player->IsBot())
            {
                auto PlayerInfo = gPugPlayer.GetInfo(EntityIndex);

                if (PlayerInfo)
                {
                    if (!PlayerInfo->m_DM.m_HideMenu)
                    {
                        gPugMenu[EntityIndex].Create("CSDM: Equipamentos", false, (void *)this->MenuHandle);

                        gPugMenu[EntityIndex].AddItem(1, "Novas Armas");

                        if ((PlayerInfo->m_DM.m_WeaponLast[PRIMARY_WEAPON_SLOT] != WEAPON_NONE) && (PlayerInfo->m_DM.m_WeaponLast[PISTOL_SLOT] != WEAPON_NONE))
                        {
                            gPugMenu[EntityIndex].AddItem(2, "Setup Anterior");

                            gPugMenu[EntityIndex].AddItem(3, "2 + Ocultar menu");
                        }

                        gPugMenu[EntityIndex].Show(EntityIndex);

                        gPugUtil.SendDHud(Player->edict(), g_DM_HudMain, "TECLE 'M'\nPARA VER O MENU DE TREINO");
                    }
                    else
                    {
                        gPugUtil.SendDHud(Player->edict(), g_DM_HudMain, "TECLE 'G'\nPARA VER O MENU DE ARMAS");

                        this->EquipItem(EntityIndex, PlayerInfo->m_DM.m_WeaponLast[PRIMARY_WEAPON_SLOT]);
                        this->EquipItem(EntityIndex, PlayerInfo->m_DM.m_WeaponLast[PISTOL_SLOT]);
                    }
                }
            }
            else
            {
                gPugDeathmatch.EquipRandomItem(EntityIndex, 2);
                gPugDeathmatch.EquipRandomItem(EntityIndex, 1);
            }
        }
    }
}

void CPugDeathmatch::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            auto PlayerInfo = gPugPlayer.GetInfo(EntityIndex);

            if (PlayerInfo)
            {
                if (Item.Info == 1)
                {
                    gPugDeathmatch.MenuEquip(EntityIndex, PISTOL_SLOT);
                    return;
                }
                else
                {
                    if (Item.Info == 3)
                    {
                        PlayerInfo->m_DM.m_HideMenu = true;

                        gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 Pressione ^3'G'^1 para exibir o menu novamente.", gPugCvar.m_Tag->string);
                    }

                    gPugDeathmatch.EquipItem(EntityIndex, PlayerInfo->m_DM.m_WeaponLast[PRIMARY_WEAPON_SLOT]);
                    gPugDeathmatch.EquipItem(EntityIndex, PlayerInfo->m_DM.m_WeaponLast[PISTOL_SLOT]);

                    return;
                }
            }

            gPugMenu[EntityIndex].Show(EntityIndex);
        }
    }
}

void CPugDeathmatch::MenuEquip(int EntityIndex, int Slot)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (this->m_Weapon.size() > 0)
        {
            gPugMenu[EntityIndex].Create("CSDM: Armas", false, (void *)this->MenuEquipHandle);

            for (auto const &Weapon : this->m_Weapon)
            {
                if (Weapon.SlotInfo)
                {
                    if (Weapon.SlotInfo->slot == Slot)
                    {
                        gPugMenu[EntityIndex].AddItem(Weapon.SlotInfo->id, Weapon.Label, false, Weapon.SlotInfo->slot);
                    }
                }
            }

            gPugMenu[EntityIndex].Show(EntityIndex);
        }
    }
}

void CPugDeathmatch::MenuEquipHandle(int EntityIndex, P_MENU_ITEM Item)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            gPugDeathmatch.EquipItem(EntityIndex, static_cast<WeaponIdType>(Item.Info));

            if (Item.Extra == 2)
            {
                gPugDeathmatch.MenuEquip(EntityIndex, PRIMARY_WEAPON_SLOT);
            }
        }
    }
}

void CPugDeathmatch::EquipItem(int EntityIndex, WeaponIdType WeaponIndex)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            if (g_ReGameApi)
            {
                auto SlotInfo = g_ReGameApi->GetWeaponSlot(WeaponIndex);

                if (SlotInfo)
                {
                    auto AutoWepSwitch = Player->m_iAutoWepSwitch;

                    Player->m_iAutoWepSwitch = 1;

                    auto NamedItem = static_cast<CBasePlayerItem *>(Player->CSPlayer()->GiveNamedItemEx(SlotInfo->weaponName));

                    if (NamedItem)
                    {
                        Player->GiveAmmo(NamedItem->CSPlayerItem()->m_ItemInfo.iMaxAmmo1, const_cast<char *>(NamedItem->CSPlayerItem()->m_ItemInfo.pszAmmo1), -1);

                        if (Player->m_pActiveItem)
                        {
                            auto PlayerInfo = gPugPlayer.GetInfo(EntityIndex);

                            if (PlayerInfo)
                            {
                                PlayerInfo->m_DM.m_WeaponLast[SlotInfo->slot] = WeaponIndex;

                                if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
                                {
                                    auto ActiveItem = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

                                    if (ActiveItem)
                                    {
                                        ActiveItem->m_iWeaponState = PlayerInfo->m_DM.m_WeaponState[ActiveItem->m_iId];
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
}

bool CPugDeathmatch::EquipRandomItem(int EntityIndex, int Slot)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

        if (Player)
        {
            if (this->m_Weapon.size() > 0)
            {
                auto Weapon = std::next(std::begin(this->m_Weapon), g_engfuncs.pfnRandomLong(0, this->m_Weapon.size() - 1));

                if (Weapon->SlotInfo)
                {
                    if (Player->IsBot() && !Weapon->Bot)
                    {
                        this->EquipRandomItem(EntityIndex, Slot);
                        return false;
                    }

                    if (Weapon->SlotInfo->slot != Slot)
                    {
                        this->EquipRandomItem(EntityIndex, Slot);
                        return false;
                    }

                    this->EquipItem(EntityIndex, Weapon->SlotInfo->id);
                    return true;
                }
            }
        }
    }

    return false;
}

void CPugDeathmatch::GetIntoGame(CBasePlayer *Player)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto PlayerInfo = gPugPlayer.GetInfo(Player->entindex());

        if (PlayerInfo)
        {
            PlayerInfo->m_DM.m_HideMenu = false;
            PlayerInfo->m_DM.m_Headshots = 0;
            
            PlayerInfo->m_DM.m_HideKillFeed = static_cast<int>(gPugCvar.m_DM_HideKillFeed->value);
            PlayerInfo->m_DM.m_HitIndicator = static_cast<int>(gPugCvar.m_DM_HitIndicator->value);
            PlayerInfo->m_DM.m_HSOnlyMode = static_cast<int>(gPugCvar.m_DM_HSOnlyMode->value);
            PlayerInfo->m_DM.m_HudKDRatio = static_cast<int>(gPugCvar.m_DM_HudKDRatio->value);
            PlayerInfo->m_DM.m_KillFade = static_cast<int>(gPugCvar.m_DM_KillFade->value);
            PlayerInfo->m_DM.m_KillSound = static_cast<int>(gPugCvar.m_DM_KillSound->value);
            PlayerInfo->m_DM.m_MoneyFrag = static_cast<int>(gPugCvar.m_DM_MoneyFrag->value);
        }
    }
}

bool CPugDeathmatch::FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (Player->m_LastHitGroup != 1)
        {
            auto Attacker = static_cast<CBasePlayer *>(Entity);

            if (Attacker)
            {
                auto PlayerInfo = gPugPlayer.GetInfo(Attacker->entindex());

                if (PlayerInfo)
                {
                    if (PlayerInfo->m_DM.m_HSOnlyMode)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void CPugDeathmatch::TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

        if (Attacker)
        {
            if (!Attacker->IsBot())
            {
                if (Attacker->entindex() != Player->entindex())
                {
                    if (g_pGameRules)
                    {
                        if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker))
                        {
                            auto PlayerInfo = gPugPlayer.GetInfo(Attacker->entindex());

                            if (PlayerInfo)
                            {
                                if (PlayerInfo->m_DM.m_HitIndicator)
                                {
                                    if (Attacker->CSEntity())
                                    {
                                        if (Attacker->CSEntity()->m_pContainingEntity)
                                        {
                                            if (gPugCvar.m_DM_HitIndicator->value >= 2.0f || Attacker->CSEntity()->m_pContainingEntity->FVisible(Player))
                                            {
                                                gPugUtil.SendDHud(Attacker->edict(), g_DM_HudHits, "%d", static_cast<int>(flDamage));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CPugDeathmatch::Killed(CBasePlayer *Victim, entvars_t *pevKiller, entvars_t *pevInflictor)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevKiller));

        if (Killer)
        {
            if (Victim->entindex() != Killer->entindex())
            {
                if (Victim->IsPlayer() && Killer->IsPlayer())
                {
                    auto RestoreHealth = gPugCvar.m_DM_KillHP->value;

                    auto HeadshotCheck = 1.0f;

                    if (Victim->m_bHeadshotKilled)
                    {
                        auto PlayerInfo = gPugPlayer.GetInfo(Killer->entindex());

                        if (PlayerInfo)
                        {
                            PlayerInfo->m_DM.m_Headshots++;
                        }

                        HeadshotCheck = 2.0f;

                        if (gPugCvar.m_DM_KillHPHS->value > 0.0f)
                        {
                            RestoreHealth = gPugCvar.m_DM_KillHPHS->value;
                        }
                    }

                    if (RestoreHealth > 0.0f)
                    {
                        Killer->edict()->v.health = clamp(Killer->edict()->v.health + RestoreHealth, 1.0f, 100.0f);
                    }

                    if (HeadshotCheck >= gPugCvar.m_DM_KillRepairArmor->value)
                    {
                        Killer->edict()->v.armorvalue = MAX_NORMAL_BATTERY;
                        Killer->m_iKevlar = ARMOR_VESTHELM;
                    }

                    if (!Killer->IsBot())
                    {
                        auto PlayerInfo = gPugPlayer.GetInfo(Killer->entindex());

                        if (PlayerInfo)
                        {
                            if (HeadshotCheck >= gPugCvar.m_DM_KillFade->value)
                            {
                                if (HeadshotCheck >= PlayerInfo->m_DM.m_KillFade)
                                {
                                    gPugUtil.ScreenFade(Killer->edict(), BIT(10), BIT(10), 0x0000, 0, 0, 200, 50);
                                }
                            }

                            if (HeadshotCheck >= gPugCvar.m_DM_KillSound->value)
                            {
                                if (HeadshotCheck >= PlayerInfo->m_DM.m_KillSound)
                                {
                                    g_engfuncs.pfnClientCommand(Killer->edict(), strdup("%s"), "spk \"sound/fvox/blip.wav\"\n");
                                }
                            }

                            if (gPugCvar.m_DM_KillHealedMsg->value)
                            {
                                auto Value = (int)(Killer->edict()->v.health - (float)(Killer->m_iLastClientHealth));

                                if (Value > 0)
                                {
                                    gPugUtil.SendDHud(Killer->edict(), g_DM_HudHeal, "%+d HP", Value);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool CPugDeathmatch::SendDeathMessage(CBaseEntity *Killer, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (Killer)
        {
            if (Killer->IsPlayer())
            {
                auto KillerInfo = gPugPlayer.GetInfo(Killer->entindex());

                if (KillerInfo)
                {
                    if (KillerInfo->m_DM.m_HideKillFeed)
                    {
                        gPugUtil.SendDeathMsg(Killer->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                    }
                }

                auto VictimInfo = gPugPlayer.GetInfo(Victim->entindex());

                if (VictimInfo)
                {
                    if (VictimInfo->m_DM.m_HideKillFeed)
                    {
                        gPugUtil.SendDeathMsg(Victim->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                    }
                }

                for (int i = 1; i <= gpGlobals->maxClients; ++i)
                {
                    auto Player = UTIL_PlayerByIndexSafe(i);

                    if (Player)
                    {
                        if (!Player->IsDormant() && Player->IsPlayer() && !Player->IsBot())
                        {
                            auto TempInfo = gPugPlayer.GetInfo(Player->entindex());

                            if (TempInfo)
                            {
                                if (!TempInfo->m_DM.m_HideKillFeed)
                                {
                                    gPugUtil.SendDeathMsg(Player->edict(), Killer, Victim, Assister, pevInflictor, killerWeaponName, iDeathMessageFlags, iRarityOfKill);
                                }
                            }
                        }
                    }
                }

                return true;
            }
        }
    }

    return false;
}

void CPugDeathmatch::UpdateClientData(CBasePlayer *Player)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (!Player->IsBot())
        {
            if (Player->IsPlayer())
            {
                if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
                {
                    auto PlayerInfo = gPugPlayer.GetInfo(Player->entindex());

                    if (PlayerInfo)
                    {
                        if (PlayerInfo->m_DM.m_HudKDRatio)
                        {
                            static float NextUpdate;

                            if (gpGlobals->time >= NextUpdate)
                            {
                                if (Player->edict()->v.frags == 0.0f)
                                {
                                    PlayerInfo->m_DM.m_Headshots = 0;
                                }

                                gPugUtil.SendDHud
                                (
                                    Player->edict(),
                                    g_DM_HudInfo,
                                    "KD: %.2f - HSP: %.2f%%",
                                    (Player->m_iDeaths > 0) ? (Player->edict()->v.frags / static_cast<float>(Player->m_iDeaths)) : (Player->edict()->v.frags > 0.0f ? 100.0f : 0.0f),
                                    (Player->edict()->v.frags > 0.0f) ? ((static_cast<float>(PlayerInfo->m_DM.m_Headshots) / Player->edict()->v.frags) * 100.0f) : 0.0f
                                );

                                NextUpdate = (gpGlobals->time + 1.0f);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool CPugDeathmatch::AddAccount(CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange)
{
    switch(gPugMod.GetState())
    {
        case STATE_WARMUP:
        {
            Player->m_iAccount = 16000;
            
            return (Type == RT_PLAYER_BOUGHT_SOMETHING);
        }
        case STATE_DEATHMATCH:
        {
            if (gPugCvar.m_DM_MoneyFrag->value)
            {
                auto PlayerInfo = gPugPlayer.GetInfo(Player->entindex());

                if (PlayerInfo)
                {
                    if (PlayerInfo->m_DM.m_MoneyFrag)
                    {
                        Player->m_iAccount = static_cast<int>(Player->edict()->v.frags);

                        return true;
                    }
                }
            }

            Player->m_iAccount = 16000;
            
            return true;
        }
    }

    return false;
}

void CPugDeathmatch::SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if ((playerAnimation == PLAYER_ATTACK1) || (playerAnimation == PLAYER_ATTACK2) || (playerAnimation == PLAYER_RELOAD))
        {
            if (Player->m_pActiveItem)
            {
                if ((Player->m_pActiveItem->m_iId == WEAPON_USP) || (Player->m_pActiveItem->m_iId == WEAPON_GLOCK18) || (Player->m_pActiveItem->m_iId == WEAPON_FAMAS) || (Player->m_pActiveItem->m_iId == WEAPON_M4A1))
                {
                    CBasePlayerWeapon *Weapon = static_cast<CBasePlayerWeapon *>(Player->m_pActiveItem);

                    if (Weapon)
                    {
                        auto PlayerInfo = gPugPlayer.GetInfo(Player->entindex());

                        if (PlayerInfo)
                        {
                            PlayerInfo->m_DM.m_WeaponState[Weapon->m_iId] = Weapon->m_iWeaponState;
                        }
                    }
                }
            }
        }
    }
}

bool CPugDeathmatch::DropItem(CBasePlayer *Player)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        auto PlayerInfo = gPugPlayer.GetInfo(Player->entindex());

        if (PlayerInfo)
        {
            if (PlayerInfo->m_DM.m_HideMenu)
            {
                PlayerInfo->m_DM.m_HideMenu = false;

                gPugUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "^4[%s]^1 O menu de armas foi habilitado.", gPugCvar.m_Tag->string);
            }

            if(!Player->m_rgpPlayerItems[PISTOL_SLOT])
            {
                gPugDeathmatch.MenuEquip(Player->entindex(), PISTOL_SLOT);
            }
            else if (!Player->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
            {
                gPugDeathmatch.MenuEquip(Player->entindex(), PRIMARY_WEAPON_SLOT);
            }
        }

        return true;
    }

    return false;
}

bool CPugDeathmatch::ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *szOldMenu)
{
    if (gPugMod.GetState() == STATE_DEATHMATCH)
    {
        if (MenuType == VGUI_Menu_Team)
        {
            if (szOldMenu)
            {
                if (_stricmp(szOldMenu, "#IG_Team_Select") == 0)
                {
                    this->MenuOption(Player->entindex());

                    return true;
                }
            }
        }
    }

    return false;
}
