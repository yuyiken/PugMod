#include "precompiled.h"

CPugAdmin gPugAdmin;

void CPugAdmin::ServerActivate()
{
    this->m_Data.clear();

    this->m_Flag.clear();

    gPugUtil.ServerCommand("exec %s/cfg/adminlist.cfg", gPugUtil.GetPath());
}

void CPugAdmin::Set(std::string Auth, std::string Name, std::string Flag)
{
    if (!Auth.empty() && !Name.empty() && !Flag.empty())
    {
        this->m_Data[Auth] = {Auth, Name, Flag};
    }
}

int CPugAdmin::ReadFlags(const char* Flags)
{
    int Result = ADMIN_ALL;

    if (Flags)
    {
        if (Flags[0u] != '\0')
        {
            while (*Flags)
            {
                Result |= BIT(*Flags++ - 'a');
            }
        }
    }

    return Result;
}

bool CPugAdmin::PutInServer(edict_t* pEntity)
{
    if (!FNullEnt(pEntity))
    {
        auto EntityIndex = ENTINDEX(pEntity);

        this->m_Flag[EntityIndex] = ADMIN_ALL;

        this->m_Flag[EntityIndex] |= this->ReadFlags("z");

        auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEntity);

        if (Auth)
        {
            if (Auth[0] != '\0')
            {
                if (this->m_Data.find(Auth) != this->m_Data.end())
                {
                    this->m_Flag[EntityIndex] |= this->ReadFlags(this->m_Data[Auth].Flag.c_str());
                }
            }
        }
    }

    return true;
}

int CPugAdmin::GetFlags(int EntityIndex)
{
    return this->m_Flag[EntityIndex];
}

int CPugAdmin::Access(int EntityIndex, int Level)
{
    if (Level == ADMIN_ADMIN)
    {
        return (this->m_Flag[EntityIndex] > ADMIN_ALL && !(this->m_Flag[EntityIndex] & ADMIN_USER));
    }
    else if (Level == ADMIN_ALL)
    {
        return 1;
    }

    return (this->m_Flag[EntityIndex] & Level);
}

int CPugAdmin::Access(std::string Auth, int Level)
{
    auto Admin = this->m_Data.find(Auth);

    if (Admin != this->m_Data.end())
    {
        auto Flags = this->ReadFlags(Admin->second.Flag.c_str());

        if (Level == ADMIN_ADMIN)
        {
            return (Flags > ADMIN_ALL && !(Flags & ADMIN_USER));
        }
        else if (Level == ADMIN_ALL)
        {
            return 1;
        }

        return (Flags & Level);
    }

    return 0;
}

std::string CPugAdmin::GetFlags(edict_t* pEdict)
{
    auto Auth = g_engfuncs.pfnGetPlayerAuthId(pEdict);

    if (Auth)
    {
        if (this->m_Data.find(Auth) != this->m_Data.end())
        {
            return this->m_Data[Auth].Flag;
        }
    }

    return "z";
}
