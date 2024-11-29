#pragma once

class CPugPlayerMenu
{
public:
    void Menu(int EntityIndex);
    static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
};

extern CPugPlayerMenu gPugPlayerMenu;