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