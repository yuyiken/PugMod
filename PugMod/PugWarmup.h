#pragma once

class CPugWarmup
{
public:
	void Init();
	void Stop();
	void RemoveMapObjective(bool Remove);
	bool AddAccount(CBasePlayer* Player, int Amount, RewardType Type, bool bTrackChange);
	bool HasRestrictItem(CBasePlayer* Player, ItemID ItemIndex, ItemRestType RestType);

private:
	bool m_Running;
	std::vector<std::vector<std::string>> m_ObjectiveEntities =
	{
		{"func_bomb_target", "_func_bomb_target"},
		{"info_bomb_target","_info_bomb_target"},
		{"func_hostage_rescue","_func_hostage_rescue"},
		{"info_hostage_rescue","_info_hostage_rescue"},
		{"info_vip_start","_info_vip_start"},
		{"func_vip_safetyzone","_func_vip_safetyzone"},
		{"func_escapezone","_func_escapezone"}
	};
};

extern CPugWarmup gPugWarmup;