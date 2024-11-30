#include "precompiled.h"

CPugVoteMap gPugVoteMap;

void CPugVoteMap::AddMap(const char* MapName)
{
    if (MapName)
    {
        if (MapName[0U] != '\0')
        {
            auto Map = strdup(MapName);

            if (g_engfuncs.pfnIsMapValid(Map))
            {
                this->m_MapList[Map] = 0;
            }
        }
    }
}

void CPugVoteMap::Init()
{
    for (auto it = this->m_MapList.begin(); it != this->m_MapList.end(); it++)
    {
        it->second = 0;
    }

    //auto Players = gPugUtil.GetPlayers(true, true);

    gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 Iniciando a escolha do novo mapa.", gPugCvar.m_Tag->string);
}

void CPugVoteMap::Stop()
{

}

void CPugVoteMap::Task()
{

}