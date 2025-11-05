#pragma once

#define _T(Key) (gPugLang.Get(Key))

class CPugLang
{
public:
    void ServerActivate();

    void Add(const char* Key, const char* Value);
    const char* Get(const char* Key);

private:
    std::unordered_map<std::string, std::string> m_Data = {};
};

extern CPugLang gPugLang;