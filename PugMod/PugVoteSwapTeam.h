#pragma once

constexpr std::array<hudtextparms_t, 2> g_VoteSwapTeam_HudParam =
{{
    {0.23f, 0.02f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 1},
    {0.23f, 0.05f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.93f, 0.93f, 2},
}};

constexpr std::array<const char*, 3> g_VoteSwapTeam_Sound =
{
    "spk gman/gman_Choose1",
    "spk gman/gman_Choose2",
    "spk scientist/letsgo",
};

typedef struct S_VOTE_SWAP_INFO
{
    TeamName Team;
    int Votes;
    std::string Name;
} P_VOTE_SWAP_INFO, *LP_VOTE_SWAP_INFO;

class CPugVoteSwapTeam
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void Init(TeamName Team);
    void Stop();
    P_VOTE_SWAP_INFO GetWinner();
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    void StartFrame();

private:
    bool m_Run = false;
    float m_NextFrame = 0.0f;
    time_t m_Time = 0;
    TeamName m_Team = UNASSIGNED;
    std::vector<P_VOTE_SWAP_INFO> m_VoteList = {};
};

extern CPugVoteSwapTeam gPugVoteSwapTeam;