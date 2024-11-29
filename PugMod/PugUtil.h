#pragma once

constexpr auto PRINT_TEAM_DEFAULT = 0;
constexpr auto PRINT_TEAM_GREY = -1;
constexpr auto PRINT_TEAM_RED = -2;
constexpr auto PRINT_TEAM_BLUE = -3;

constexpr auto PRINT_NOTIFY = 1;
constexpr auto PRINT_CONSOLE = 2;
constexpr auto PRINT_CHAT = 3;
constexpr auto PRINT_CENTER = 4;
constexpr auto PRINT_RADIO = 5;

class CPugUtil
{
public:
    void ServerCommand(const char *Format, ...);
    int ParseColors(char *Buffer);
    void SayText(edict_t *pEntity, int Sender, const char *Format, ...);
    void ClientPrint(edict_t *pEntity, int iMsgDest, const char *Format, ...);
    unsigned short FixedUnsigned16(float Value, float Scale);
    short FixedSigned16(float Value, float Scale);
    hudtextparms_t SetHud(vec3_t FromColor, vec3_t ToColor, float X, float Y, int Effect, float FxTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel);
    void SendHud(edict_t *pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void SendDHud(edict_t* pEntity, const hudtextparms_t &TextParams, const char *Format, ...);
    void ScreenFade(edict_t *pEntity, unsigned short Duration, unsigned short HoldTime, short FadeFlags, short Red, short Green, short Blue, short Alpha);
    void TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName);
    void SendDeathMsg(edict_t* pEntity, CBaseEntity* pKiller, CBasePlayer* pVictim, CBasePlayer* pAssister, entvars_t* pevInflictor, const char* killerWeaponName, int iDeathMessageFlags, int iRarityOfKill);
    std::vector<CBasePlayer*> GetPlayers(bool InGame, bool Bots);

};

extern CPugUtil gPugUtil;