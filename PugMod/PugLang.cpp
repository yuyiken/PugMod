#include "precompiled.h"

CPugLang gPugLang;

void CPugLang::ServerActivate()
{
    this->m_Data.clear();

    gPugUtil.ServerCommand("exec %s/cfg/language.cfg", gPugUtil.GetPath());
}

void CPugLang::Add(const char* Key, const char* Value)
{
    if (Key && Value)
    {
        if (Key[0u] != '\0' && Value[0u] != '\0')
        {
            this->m_Data[Key] = Value;
        }
    }
}

const char* CPugLang::Get(const char* Key)
{
    if (Key)
    {
        if (!this->m_Data.empty())
        {
            if (this->m_Data.find(Key) != this->m_Data.end())
            {
                return this->m_Data[Key].c_str();
            }
        }
    }

    return Key;
}