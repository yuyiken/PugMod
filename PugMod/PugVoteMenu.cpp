#include "precompiled.h"

CPugVoteMenu gPugVoteMenu;

void CPugVoteMenu::ServerActivate()
{

}

bool CPugVoteMenu::Menu(CBasePlayer *Player)
{
    if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
    {
        auto State = gPugMod.GetState();

        if (State != STATE_DEAD && State != STATE_VOTEMAP && State != STATE_VOTETEAM && State != STATE_CAPTAIN && State != STATE_END)
        {
            gPugMenu[Player->entindex()].Create(true, E_MENU::ME_PLAYER_VOTE_MENU, _T("Vote Menu:"));

            gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_KICK, false, 0, _T("Kick Player"));
            gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_MAP, false, 0, _T("Change Map"));

            if (State >= STATE_FIRST_HALF && State <= STATE_OVERTIME)
            {
                gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_PAUSE, false, 0, _T("Pause Match"));
                gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_RESTART, false, 0, _T("Restart Match"));
                gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_CANCEL, false, 0, _T("Cancel Match"));
                gPugMenu[Player->entindex()].AddItem(PLAYER_VOTE_SURRENDER, false, 0, _T("Vote Surrender"));
            }

            gPugMenu[Player->entindex()].Show(Player);

            gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Choose an option to vote."), gPugCvar.m_Tag->string);

            return true;
        }
    }

    gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Command unavailable."), gPugCvar.m_Tag->string);

    return false;
}

void CPugVoteMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    switch (Item.Info)
    {
        case PLAYER_VOTE_KICK:
        {
            this->VoteKick(Player);
            break;
        }
        case PLAYER_VOTE_MAP:
        {
            this->VoteMap(Player);
            break;
        }
        case PLAYER_VOTE_PAUSE:
        {
            this->VotePause(Player);
            break;
        }
        case PLAYER_VOTE_RESTART:
        {
            this->VoteRestart(Player);
            break;
        }
        case PLAYER_VOTE_CANCEL:
        {
            this->VoteCancel(Player);
            break;
        }
        case PLAYER_VOTE_SURRENDER:
        {
            this->VoteSurrender(Player);
            break;
        }
    }
}

bool CPugVoteMenu::VoteKick(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VoteKickHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}

bool CPugVoteMenu::VoteMap(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VoteMapHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}

bool CPugVoteMenu::VotePause(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VotePauseHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}

bool CPugVoteMenu::VoteRestart(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VoteRestartHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}

bool CPugVoteMenu::VoteCancel(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VoteCancelHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}

bool CPugVoteMenu::VoteSurrender(CBasePlayer *Player)
{
    return false;
}

void CPugVoteMenu::VoteSurrenderHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{

}