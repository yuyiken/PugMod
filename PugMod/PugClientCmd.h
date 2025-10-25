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
    CMD_HELP,
    CMD_HELP_ADMIN,
    CMD_ADMIN_MENU,
    CMD_ADMIN_KICK,
    CMD_ADMIN_BAN,
    CMD_ADMIN_SLAP,
    CMD_ADMIN_TEAM,
    CMD_ADMIN_MESSAGE,
    CMD_ADMIN_RCON,
    CMD_ADMIN_CVAR
};

typedef struct S_CLIENT_CMD
{
    int Index;
    std::string Name;
    int Flags;
} P_CLIENT_CMD, *LP_CLIENT_CMD;

class CPugClientCmd
{
public:
    void ServerActivate();

    void Set(std::string Index, std::string Name, std::string Flags);
    LP_CLIENT_CMD Get(const char* Name);
    LP_CLIENT_CMD Get(int Index);

    bool FilterFlood(const char *pszCommand, int EntityIndex);
    bool Command(edict_t *pEntity);

    bool Help(CBasePlayer *Player);
    bool HelpAdmin(CBasePlayer *Player);
    bool Message(CBasePlayer *Player);
    bool Rcon(CBasePlayer *Player);
private:
    std::map<std::string, P_CLIENT_CMD> m_Data = {};

    std::array<float, MAX_CLIENTS + 1> m_FloodCount = {};
    std::array<float, MAX_CLIENTS + 1> m_FloodTimer = {};
};

extern CPugClientCmd gPugClientCmd;