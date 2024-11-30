#pragma once

class CPugVoteMap
{
public:
    void AddMap(const char* MapName);

    void Init();
    void Stop();
    void Task();

private:
    std::map<std::string, int> m_MapList;
};

extern CPugVoteMap gPugVoteMap;