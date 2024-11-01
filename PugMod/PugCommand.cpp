#include "precompiled.h"

CPugCommand gPugCommand;

bool CPugCommand::ClientCommand(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

		if (Player)
		{
			auto pCmd = g_engfuncs.pfnCmd_Argv(0);

			if (pCmd)
			{
				auto pArg1 = g_engfuncs.pfnCmd_Argv(1);

				if (pCmd[0u] != '\0')
				{
					if (!Q_strcmp(pCmd, "menuselect"))
					{
						if (pArg1)
						{
							if (Player->m_iMenu == Menu_OFF)
							{
								if (gPugMenu[Player->entindex()].Handle(Player->entindex(), atoi(pArg1)))
								{
									return true;
								}
							}
						}
					}
					else if (!Q_strcmp(pCmd, "drop"))
					{
						if (gPugDeathmatch.SetHideMenu(Player, false))
						{
							return true;
						}
					}
					else if (!Q_strcmp(pCmd, "say") || !Q_strcmp(pCmd, "say_team"))
					{
						if (pArg1)
						{
							if (pArg1[0u] != '\0')
							{
								if (Q_strstr(pArg1, "guns") != nullptr)
								{
									if (gPugDeathmatch.SetHideMenu(Player, false))
									{
										return true;
									}
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
