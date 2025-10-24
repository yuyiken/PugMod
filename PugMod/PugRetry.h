#pragma once

class CPugRetry
{
public:
    void PutInServer(edict_t *pEntity);
    void DropClient(edict_t *pEntity, bool crash, const char *Reason);

private:
    std::map<std::string, float> m_Data;
};

extern CPugRetry gPugRetry;
