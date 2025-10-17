#include "precompiled.h"

CPugMapList gPugMapList;

void CPugMapList::ServerActivate()
{
    this->m_Data.clear();
}

void CPugMapList::Add(const char* Map)
{
    if (Map)
    {
        if (Map[0u] != '\0')
        {
            char *Name = strdup(Map);

            if (g_engfuncs.pfnIsMapValid(Name))
            {
                this->m_Data.insert(std::make_pair(this->m_Data.size(), Name));
            }

            free(Name);
        }
    }
}

std::map<int, std::string> CPugMapList::Get()
{
    return this->m_Data;
}