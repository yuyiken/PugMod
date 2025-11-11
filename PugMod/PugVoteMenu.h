#pragma once

constexpr auto PLAYER_VOTE_KICK = 0;
constexpr auto PLAYER_VOTE_MAP = 1;
constexpr auto PLAYER_VOTE_PAUSE = 2;
constexpr auto PLAYER_VOTE_RESTART = 3;
constexpr auto PLAYER_VOTE_CANCEL = 4;
constexpr auto PLAYER_VOTE_SURRENDER = 5;

class CPugVoteMenu
{
public:
    void ServerActivate();

    bool Menu(CBasePlayer *Player);
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteKick(CBasePlayer *Player);
    void VoteKickHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteMap(CBasePlayer *Player);
    void VoteMapHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VotePause(CBasePlayer *Player);
    void VotePauseHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteRestart(CBasePlayer *Player);
    void VoteRestartHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteCancel(CBasePlayer *Player);
    void VoteCancelHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteSurrender(CBasePlayer *Player);
    void VoteSurrenderHandle(CBasePlayer *Player, P_MENU_ITEM Item);
};

extern CPugVoteMenu gPugVoteMenu;