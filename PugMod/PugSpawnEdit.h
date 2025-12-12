#pragma once

class CPugSpawnEdit
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void EditSpawns(CBasePlayer* Player);
	void Menu(CBasePlayer* Player);
	void MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item);
	void AddSpawnMenu(CBasePlayer* Player);
	void AddSpawnMenuHandle(CBasePlayer* Player, P_MENU_ITEM Item);
	void ToggleNoClip(CBasePlayer* Player);
	void DiscardChanges(CBasePlayer* Player);
	void DiscardChangesHandle(CBasePlayer* Player, P_MENU_ITEM Item);
	edict_t* MakeEntity(int EntityIndex);
	void RemoveEntity(int EntityIndex);
	void GlowEnt(int EntityIndex, vec3_t Color);
	void UnGlowEnt(int EntityIndex);
	bool IsStuck(int EntityIndex);
	void SetPosition(int EntityIndex);
	void AddSpawn(entvars_t v, int Team, float FixOrigin);
	void EditSpawn(CBasePlayer *Player, entvars_t v, float FixOrigin);
	void DeleteSpawn(int EntityIndex);
	int ClosestSpawn(edict_t* pEdict);
	void Refresh(CBasePlayer* Player);
	void ShowStats(CBasePlayer* Player);
	void Save(CBasePlayer* Player);
	void ShowStuckedSpawns(CBasePlayer* Player);

private:
	string_t m_InfoTarget = 0;
	std::map<size_t, P_SPAWN> m_Spawns = {};
	std::map<int, edict_t*> m_Entities = {};
	std::map<int, int> m_Marked = {};
};

extern CPugSpawnEdit gPugSpawnEdit;