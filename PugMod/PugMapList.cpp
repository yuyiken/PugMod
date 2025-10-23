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

            if (g_engfuncs.pfnIsMapValid(File))
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