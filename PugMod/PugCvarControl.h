#pragma once

class CPugCvarControl
{
public:
    void ServerActivate();
    void Add(const char *Name, const char *String);

private:
    std::map<std::string, std::vector<std::string>> m_Data;
};

extern CPugCvarControl gPugCvarControl;
