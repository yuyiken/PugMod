#pragma once

#define BIT_IS_LEADER   BIT(24)
#define BIT_IS_INMENU   BIT(25)

constexpr std::array<hudtextparms_t, 6> g_Captain_HudParam =
{{
    {0.75f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1}, // Terroristas (%d / %d):
    {0.75f, 0.02f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2}, // \n%s

    {0.75f, 0.28f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 3}, // CTs (%d / %d):
    {0.75f, 0.28f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 4}, // \n%s

    {0.75f, 0.28f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 3}, // CTs (%d / %d):\n\n\n\n\n\nEspectadores (%d):
    {0.75f, 0.28f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 4}, // \n%s\n%s
}};

class CPugCaptain
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void Init();
    void Stop();

    void DropClient(edict_t *pEntity);

    void SetCaptain(CBasePlayer *Player, TeamName Team);
    void GetPlayer(CBasePlayer *Captain, CBasePlayer *Target);
    void NextMenu(CBasePlayer *Captain);

    void Menu(CBasePlayer *Captain);
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void GetRandomPlayer(int EntityIndex);
    void StartFrame();

private:
    bool m_Run = false;
    float m_NextFrame = 0.0f;
    int m_PlayersMin = 0;
    std::map<int, TeamName> m_Team;
};

extern CPugCaptain gPugCaptain;