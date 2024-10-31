#include "precompiled.h"

CPugWarmup gPugWarmup;

void CPugWarmup::Init()
{
	this->m_Running = true;

	this->RemoveMapObjective(true);

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotDeathmatch, "1");

	gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 O Modo Deathmatch está ativo até o início da partida.", Plugin_info.logtag);
}

void CPugWarmup::Stop()
{
	this->m_Running = false;

	this->RemoveMapObjective(false);

	g_engfuncs.pfnCvar_DirectSet(gPugCvar.m_BotDeathmatch, "0");

	gPugUtil.SayText(nullptr, PRINT_TEAM_DEFAULT, "^4[%s]^1 Modo Deathmatch desativado.", Plugin_info.logtag);
}

void CPugWarmup::RemoveMapObjective(bool Remove)
{
	if (gReGameDLL.m_Funcs)
	{
		CBaseEntity* Entity = nullptr;

		for (auto& Name : this->m_ObjectiveEntities)
		{
			while ((Entity = gReGameDLL.m_Funcs->UTIL_FindEntityByString(Entity, "classname", Remove ? Name[0].c_str() : Name[1].c_str())) != nullptr)
			{
				Entity->edict()->v.classname = g_engfuncs.pfnAllocString(Remove ? Name[1].c_str() : Name[0].c_str());
			}
		}
	}
}

bool CPugWarmup::AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange)
{
	if (this->m_Running)
	{
		if (this->m_Running && Type == RT_PLAYER_BOUGHT_SOMETHING)
		{
			return true;
		}
	}
	
	return false;
}

bool CPugWarmup::HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType)
{
	if (this->m_Running)
	{
		switch (ItemIndex)
		{
			case ITEM_SHIELDGUN:
			case ITEM_HEGRENADE:
			case ITEM_C4:
			case ITEM_SMOKEGRENADE:
			case ITEM_FLASHBANG:
			case ITEM_DEFUSEKIT:
			{
				gPugUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
				return true;
			}
		}
	}

	return false;
}