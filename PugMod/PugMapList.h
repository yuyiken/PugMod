#pragma once

class CPugMapList
{
public:
    void ServerActivate();
    void Add(const char *Map);
    std::map<int, std::string> Get();

private:
    std::map<int, std::string> m_Data;    
};

extern CPugMapList gPugMapList;