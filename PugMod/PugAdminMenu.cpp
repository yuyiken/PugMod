#include "precompiled.h"

CPugAdminMenu gPugAdminMenu;

void CPugAdminMenu::Menu(CBasePlayer *Player)
{
    auto Flags = gPugAdmin.GetFlags(Player->entindex());

    if (!(Flags & ADMIN_MENU))
    {
        gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, "^4[%s]^1 Você não tem acesso a esse comando.", gPugCvar.m_Tag->string);
        return;
    }

    gPugMenu[Player->entindex()].Create("Menu do Pug Mod:", true, E_MENU::ME_ADMIN_MENU);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_CTRL_PLAYER, "Controlar Jogadores", false, 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_CTRL_SERVER, "Controlar Servidor", false, 0); // Adicionar Trocar Mapa, Adicionar Swap Teams!

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SEND_MSG, "Enviar Mensagem", false, 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_SEND_CMD, "Enviar Comando", false, 0);

    gPugMenu[Player->entindex()].AddItem(ADMIN_MENU_CTRL_CVAR, "Menu de Cvars", false, 0);

    gPugMenu[Player->entindex()].Show(Player);
}

void CPugAdminMenu::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        switch (Item.Info)
        {
            case ADMIN_MENU_CTRL_PLAYER:
            {
                break;
            }
            case ADMIN_MENU_CTRL_SERVER:
            {
                break;
            }
            case ADMIN_MENU_SEND_MSG:
            {
                break;
            }
            case ADMIN_MENU_SEND_CMD:
            {
                break;
            }
            case ADMIN_MENU_CTRL_CVAR:
            {
                break;
            }
        }
    }
}
