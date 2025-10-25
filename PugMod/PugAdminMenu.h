#pragma once

constexpr auto ADMIN_MENU_KICK = 0;
constexpr auto ADMIN_MENU_BAN = 1;
constexpr auto ADMIN_MENU_SLAP = 2;
constexpr auto ADMIN_MENU_TEAM = 3;
constexpr auto ADMIN_MENU_MAP = 4;
constexpr auto ADMIN_MENU_PUG = 5;
constexpr auto ADMIN_MENU_CVAR = 6;

class CPugAdminMenu
{
public:
    void Menu(CBasePlayer *Player);
    void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void Kick(CBasePlayer *Player);
    void KickHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void Ban(CBasePlayer *Player);
    void BanHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void Slap(CBasePlayer *Player);
    void SlapHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void Team(CBasePlayer *Player);
    void TeamHandle(CBasePlayer *Player, P_MENU_ITEM Item);

    void Map(CBasePlayer *Player);
    void MapHandle(CBasePlayer *Player, P_MENU_ITEM Item);
    void ChangeMap(int Index);

    void Pug(CBasePlayer *Player);
    void PugHandle(CBasePlayer *Player, P_MENU_ITEM Item);
};

extern CPugAdminMenu gPugAdminMenu;