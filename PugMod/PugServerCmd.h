#pragma once

class CPugServerCmd
{
public:
	void ServerActivate();

	static void CSDM_AddSpawn();
	static void CSDM_AddItem();
};

extern CPugServerCmd gPugServerCmd;