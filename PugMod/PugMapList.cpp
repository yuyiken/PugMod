#include "precompiled.h"

CPugMapList gPugMapList;

void CPugMapList::ServerActivate()
{
    this->m_Data.clear();
}

void CPugMapList::Add(const char *Map)
{
    if (Map)
    {
        if (Map[0u] != '\0')
        {
            char *File = strdup(Map);

            if (IS_MAP_VALID(File))
            {
                this->m_Data.insert(std::make_pair(this->m_Data.size(), Map));
            }

            free(File);
        }
    }
}

std::map<int, std::string> CPugMapList::Get()
{
    return this->m_Data;
}

void CPugMapList::ChangeMap(int Index)
{
    auto MapList = gPugMapList.Get();

    if (!MapList.empty())
    {
        auto it = MapList.find(Index);

        if (it != MapList.end())
        {
            g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_VoteMap, "0");

            gPugUtil.ServerCommand("changelevel %s", it->second.c_str());
        }
    }
}
