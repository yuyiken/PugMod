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
    cvar_t* CvarRegister(std::string Name, std::string Value);
    void ServerCommand(const char* Format, ...);
    int ParseColors(char* szBuffer);
    void ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...);
    void SayText(edict_t* pEntity, int Sender, const char* Format, ...);
    unsigned short FixedUnsigned16(float value, float scale);
    short FixedSigned16(float value, float scale);
    hudtextparms_t HudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel);
    void HudMessage(edict_t* pEntity, hudtextparms_t textparms, const char* Format, ...);
    void TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName);
    std::array<int, SPECTATOR + 1U> GetPlayerCount();

public:
    std::map<std::string, cvar_t> m_CvarData;
};

extern CPugUtil gPugUtil;