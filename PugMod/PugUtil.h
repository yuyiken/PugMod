#pragma once

enum E_PRINT_TEAM
{
    DEFAULT = 0,
    GREY    = -1,
    RED     = -2,
    BLUE    = -3
};

enum E_PRINT
{
    NOTIFY = 1,
    CONSOLE = 2,
    CHAT = 3,
    CENTER = 4,
    RADIO = 5
};

class CPugUtil
{
public:
    const char *GetPath();
    void ServerCommand(const char *Format, ...);
    bool IsNetClient(edict_t *pEntity);
    void PrintColor(edict_t *pEntity, int Sender, const char *Format, ...);
    void TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName);
    void ParseColor(char *Buffer);
    void ClientPrint(edict_t *pEntity, int iMsgDest, const char *Format, ...);
    void ClientCommand(edict_t *pEntity, const char *Format, ...);
    unsigned short FixedUnsigned16(float Value, float Scale);
    short FixedSigned16(float Value, float Scale);
    hudtextparms_t SetHud(vec3_t FromColor, vec3_t ToColor, float X, float Y, int Effect, float FxTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel);
    void SendHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void SendDHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void SendDeathMsg(edict_t *pEntity, CBaseEntity *pKiller, CBasePlayer *pVictim, CBasePlayer *pAssister, entvars_t *pevInflictor, const char *killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
    void ScreenFade(edict_t *pEntity, unsigned short Duration, unsigned short HoldTime, short FadeFlags, short Red, short Green, short Blue, short Alpha);
    std::vector<CBasePlayer *> GetPlayers(bool InGame, bool Bots);
    std::vector<CBasePlayer *> GetPlayers(TeamName Team);
    std::map<int, std::vector<CBasePlayer *>> GetPlayers();
    
private:
    std::string m_Path;
};

extern CPugUtil gPugUtil;