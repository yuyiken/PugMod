#pragma once

class CPugDemoRecord
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void PlayerSpawn(CBasePlayer *Player);
private:
    std::array<bool, MAX_CLIENTS + 1> m_Recording = {};
};

extern CPugDemoRecord gPugDemoRecord;