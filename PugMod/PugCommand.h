#pragma once

class CPugCommand
{
public:
	bool ClientCommand(edict_t* pEntity);
};

extern CPugCommand gPugCommand;