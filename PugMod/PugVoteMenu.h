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

    void DropClient(edict_t *pEntity);

    bool Menu(CBasePlayer *Player);
    bool MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteKick(CBasePlayer *Player);
    bool VoteKickHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteMap(CBasePlayer *Player);
    bool VoteMapHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VotePause(CBasePlayer *Player);
    bool VotePauseHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteRestart(CBasePlayer *Player);
    bool VoteRestartHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteCancel(CBasePlayer *Player);
    bool VoteCancelHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    bool VoteSurrender(CBasePlayer *Player);
    bool VoteSurrenderHandle(CBasePlayer *Player, P_MENU_ITEM Item);

private:
    std::array<std::array<bool, MAX_CLIENTS + 1U>, MAX_CLIENTS + 1U> m_VoteKick = {};
    std::array<std::array<bool, MAX_VOTE_MAPS + 1U>, MAX_CLIENTS + 1U> m_VoteMap = {};
};

extern CPugVoteMenu gPugVoteMenu;