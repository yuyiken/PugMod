#pragma once

constexpr auto ADMIN_MENU_CTRL_PLAYER = 0;
constexpr auto ADMIN_MENU_CTRL_SERVER = 1;
constexpr auto ADMIN_MENU_SEND_MSG = 2;
constexpr auto ADMIN_MENU_SEND_CMD = 3;
constexpr auto ADMIN_MENU_CTRL_CVAR = 4;

class CPugAdminMenu
{
public:
    void Menu(CBasePlayer *Player);
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void MenuPlayer(CBasePlayer *Player);
    void MenuPlayerHandle(CBasePlayer *Player, P_MENU_ITEM Item);
};

extern CPugAdminMenu gPugAdminMenu;