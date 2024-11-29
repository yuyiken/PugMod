#pragma once

typedef struct S_PLAYER_INFO
{
    P_PLAYER_DM m_DM;

    void Clear()
    {
        this->m_DM.Clear();
    }
} P_PLAYER_INFO, *LP_PLAYER_INFO;

class CPugPlayer
{
public:
    LP_PLAYER_INFO GetInfo(int EntityIndex);

    void PutInServer(edict_t *pEntity);
    void ClientDisconnect(edict_t *pEntity);

    bool HandleMenu_ChooseTeam(CBasePlayer *Player, int Slot);
    void GetIntoGame(CBasePlayer *Player);
    void UpdateClientData(CBasePlayer *Player);
    bool AddAccount(CBasePlayer *Player, int Amount, RewardType Type, bool TrackChange);
    bool HasRestrictItem(CBasePlayer *Player, ItemID item, ItemRestType type);
    bool GetPlayerSpawnSpot(CBasePlayer *Player);
    void PlayerSpawn(CBasePlayer *Player);
    void GiveDefaultItems(CBasePlayer *Player);
    bool FPlayerCanTakeDamage(CBasePlayer *Player, CBaseEntity *Entity);
    void TakeDamage(CBasePlayer *Player, entvars_t *pevInflictor, entvars_t *pevAttacker, float &flDamage, int bitsDamageType);
    void Killed(CBasePlayer *Victim, entvars_t *pevKiller, entvars_t *pevInflictor);
    bool SendDeathMessage(CBaseEntity *Killer, CBasePlayer *Victim, CBasePlayer *Assister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
    bool ClientCommand(edict_t *pEntity);
    void SetAnimation(CBasePlayer *Player, PLAYER_ANIM playerAnimation);
    bool ShowVGUIMenu(CBasePlayer *Player, int MenuType, int BitMask, char *szOldMenu);

private:
    std::array<P_PLAYER_INFO, MAX_CLIENTS + 1> m_Info;
};

extern CPugPlayer gPugPlayer;