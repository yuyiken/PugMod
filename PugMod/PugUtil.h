#pragma once

class CPugUtil
{
public:
    cvar_t* CvarRegister(std::string Name, std::string Value);

    std::map<std::string, cvar_t> m_CvarData;
};

extern CPugUtil gPugUtil;