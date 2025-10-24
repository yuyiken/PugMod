#pragma once

class CPugAutoRecord
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void GetIntoGame(CBasePlayer *Player);
    void PlayerSpawn(CBasePlayer *Player);

private:
    std::array<bool, MAX_CLIENTS + 1> m_Recording = {};
};

extern CPugAutoRecord gPugAutoRecord;