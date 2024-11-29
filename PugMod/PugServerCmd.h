#pragma once

class CPugServerCmd
{
public:
    void ServerActivate();

    static void Status();
    static void AddWeapon();
};

extern CPugServerCmd gPugServerCmd;