#include "precompiled.h"

CPugCvarControl gPugCvarControl;

void CPugCvarControl::ServerActivate()
{
    this->m_Data.clear();

    gPugUtil.ServerCommand("exec %s/cfg/cvars.cfg", gPugUtil.GetPath());
}

void CPugCvarControl::Add(const char *Name, const char *String)
{
    if (Name)
    {
        if (Name[0u] != '\0')
        {
            if (g_engfuncs.pfnCVarGetPointer(Name))
            {
                std::string Temp = "";

                if (String)
                {
                    if (String[0u] != '\0')
                    {
                        Temp = String;
                    }
                }

                this->m_Data[Name].push_back(Temp);
            }
        }
    }
}

void CPugCvarControl::Menu(CBasePlayer *Player)
{
    if (gPugAdmin.CheckAccess(Player, ADMIN_CVAR))
    {
        gPugMenu[Player->entindex()].Create("Controle de Cvars", true, E_MENU::ME_ADMIN_MENU_CVAR);

        for (auto Item : this->m_Data)
        {
            auto Pointer = g_engfuncs.pfnCVarGetPointer(Item.first.c_str());

            if (Pointer)
            {
                gPugMenu[Player->entindex()].AddItemFormat(0, false, 0, "%s \\R\"%s\"", Pointer->name, Pointer->string);
            }
        }

        gPugMenu[Player->entindex()].Display(Player, 0);
    }
}

void CPugCvarControl::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
    if (Player)
    {
        /**/
    }
}