#include "precompiled.h"

CPugClientCmd gPugClientCmd;

bool CPugClientCmd::Command(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		if (Player)
		{
			auto pCmd = g_engfuncs.pfnCmd_Argv(0);

			if (pCmd)
			{
				if (pCmd[0u] != '\0')
				{
					if (!Q_strcmp(pCmd, "menuselect"))
					{
						auto pArg1 = g_engfuncs.pfnCmd_Argv(1);

						if (pArg1)
						{
							if (gPugMenu[Player->entindex()].Handle(Player->entindex(), std::atoi(pArg1)))
							{
								return true;
							}
						}
					}
					else if (!Q_strcmp(pCmd, "drop"))
					{
						if (gPugDeathmatch.SetEquipMenu(Player, true))
						{
							return true;
						}
					}
					else if (!Q_strcmp(pCmd, "say") || !Q_strcmp(pCmd, "say_team"))
					{
						auto pArg1 = g_engfuncs.pfnCmd_Argv(1);

						if (pArg1)
						{
							if (pArg1[0u] != '\0')
							{
								if (!Q_strcmp(pArg1, "/guns"))
								{
									if (gPugDeathmatch.SetEquipMenu(Player, true))
									{
										return true;
									}
								}

								if (!Q_strcmp(pArg1, "/menu"))
								{
									gPugDeathmatch.OptionMenu(Player->entindex());
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}
