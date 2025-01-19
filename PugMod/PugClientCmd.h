#pragma once

enum E_CLIENT_CMD_ID
{
    CMD_STATUS,
    CMD_SCORE,
    CMD_READY,
    CMD_NOTREADY,
    CMD_MENU,
    CMD_GUNS,
    CMD_RESPAWN,
    CMD_RESET_SCORE,
    CMD_DM_SPAWN_EDITOR,
};

typedef struct S_CLIENT_CMD
{
    int Index;
    std::string Name;
    int Flags;
} P_CLIENT_CMD, *LP_CLIENT_CMD;

constexpr auto CMD_MAX_FLOOD_REPEAT = 4;
constexpr auto CMD_MIN_FLOOD_TIME = 0.75f;
constexpr auto CMD_MIN_FLOOD_NEXT_TIME = 4.0f;

class CPugClientCmd
{
public:
    void ServerActivate();

    void Set(std::string Index, std::string Name, std::string Flags);
    LP_CLIENT_CMD Get(const char* Name);
    LP_CLIENT_CMD Get(int Index);

    bool FilterFlood(const char *pszCommand, int EntityIndex);
    bool Command(edict_t *pEntity);

private:
    std::map<std::string, P_CLIENT_CMD> m_Data;

    std::array<short, MAX_CLIENTS + 1> m_FloodCount;
    std::array<float, MAX_CLIENTS + 1> m_FloodTimer;
};

extern CPugClientCmd gPugClientCmd;