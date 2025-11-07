#include "precompiled.h"

CPugSpawnEdit gPugSpawnEdit;

void CPugSpawnEdit::ServerActivate()
{
	this->m_Spawns.clear();

	this->m_Entities.clear();

	this->m_Marked.clear();

	this->m_InfoTarget = g_engfuncs.pfnAllocString("info_target");
}

void CPugSpawnEdit::ServerDeactivate()
{
	this->m_Spawns.clear();

	this->m_Entities.clear();

	this->m_Marked.clear();
}

void CPugSpawnEdit::EditSpawns(CBasePlayer* Player)
{
	if (gPugAdmin.CheckAccess(Player, ADMIN_LEVEL_D))
	{
		if (gPugMod.GetState() == STATE_DEATHMATCH)
		{
			if (this->m_Spawns.empty())
			{
				this->m_Spawns = gPugDM.GetSpawns();
			}
			
			this->MakeEntity(-1);

			this->Menu(Player);
		}
		else
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Comando indisponível."), gPugCvar.m_Tag->string);
		}
	}
}

void CPugSpawnEdit::Menu(CBasePlayer* Player)
{
	gPugMenu[Player->entindex()].Create(true, E_MENU::DM_SPAWN_EDIT, _T("CSDM: Gerenciador de Spawns"));

	gPugMenu[Player->entindex()].AddItem(1, false, 0, _T("Adicionar Spawn na posição atual"));

	if (this->m_Spawns.empty())
	{
		gPugMenu[Player->entindex()].AddItem(2, true, 0, _T("Editar Spawn - Nenhum Spawn"));
		gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Apagar Spawn - Nenhum Spawn"));
	}
	else if (FNullEnt(this->m_Entities[this->m_Marked[Player->entindex()]]))
	{
		gPugMenu[Player->entindex()].AddItem(2, true, 0, _T("Editar Spawn - Nenhum Spawn Marcado"));
		gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Apagar Spawn - Nenhum Spawn Marcado"));
	}
	else
	{
		gPugMenu[Player->entindex()].AddItem(2, false, 0, _T("Mover Spawn mais próximo (^yAmarelo^w) para a posição atual"));

		if ((Player->edict()->v.origin - this->m_Entities[this->m_Marked[Player->entindex()]]->v.origin).Length() > 200.0f)
		{
			gPugMenu[Player->entindex()].AddItem(3, true, 0, _T("Apagar Spawn - Spawn Selecionado muito longe"));
		}
		else
		{
			gPugMenu[Player->entindex()].AddItem(3, false, 0, _T("Apagar Spawn mais próximo"));
		}
	}

	gPugMenu[Player->entindex()].AddItem(4, false, 0, _T("Selecionar Spawn mais próximo"));

	gPugMenu[Player->entindex()].AddItem(5, false, 0, _T("Exibir Estatísticas"));

	if (Player->edict()->v.movetype == MOVETYPE_WALK)
	{
		gPugMenu[Player->entindex()].AddItem(6, false, 0, _T("Ativar No Clip"));
	}
	else if(Player->edict()->v.movetype == MOVETYPE_NOCLIP)
	{
		gPugMenu[Player->entindex()].AddItem(6, false, 0, _T("Desativar No Clip"));
	}

	gPugMenu[Player->entindex()].AddItem(7, false, 0, _T("Descartar todas alterações, e sair"));

	gPugMenu[Player->entindex()].AddItem(8, false, 0, _T("Salvar todas as alterações"));

	gPugMenu[Player->entindex()].Show(Player);
}

void CPugSpawnEdit::MenuHandle(CBasePlayer *Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		if (!Item.Disabled)
		{
			switch (Item.Info)
			{
				case 1:
				{
					this->AddSpawnMenu(Player);
					break;
				}
				case 2:
				{
					this->EditSpawn(Player, Player->edict()->v, 15.0f);

					this->Menu(Player);
					break;
				}
				case 3:
				{
					this->DeleteSpawn(Player->entindex());

					this->Menu(Player);
					break;
				}
				case 4:
				{
					this->Refresh(Player);
					this->Menu(Player);
					break;
				}
				case 5:
				{
					this->ShowStats(Player);
					this->Menu(Player);
					break;
				}
				case 6:
				{
					if (Player->edict()->v.movetype == MOVETYPE_WALK)
					{
						Player->edict()->v.movetype = MOVETYPE_NOCLIP;

						gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::BLUE, _T("^4[%s]^1 ^3No-Clip Habilitado."), gPugCvar.m_Tag->string);
					}
					else if (Player->edict()->v.movetype == MOVETYPE_NOCLIP)
					{
						Player->edict()->v.movetype = MOVETYPE_WALK;

						gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3No-Clip Desabilitado."), gPugCvar.m_Tag->string);
					}

					this->Menu(Player);
					break;
				}
				case 7:
				{
					this->DiscardChanges(Player);
					break;
				}
				case 8:
				{
					this->Save(Player);

					this->Menu(Player);
					break;
				}
			}
		}
	}
}

void CPugSpawnEdit::AddSpawnMenu(CBasePlayer* Player)
{
	gPugMenu[Player->entindex()].Create(false, E_MENU::DM_SPAWN_EDIT_ADD_SPAWN, _T("CSDM: Menu de Adicionar Spawn"));

	gPugMenu[Player->entindex()].AddItem(1, false, 0, _T("Adicionar posição atual como um Spawn Aleatório"));
	gPugMenu[Player->entindex()].AddItem(2, false, 0, _T("Adicionar posição atual como um Spawn de TR"));
	gPugMenu[Player->entindex()].AddItem(3, false, 0, _T("Adicionar posição atual como um Spawn de CT"));
	gPugMenu[Player->entindex()].AddItem(4, false, 0, _T("Cancelar"));

	gPugMenu[Player->entindex()].Show(Player);
}

void CPugSpawnEdit::AddSpawnMenuHandle(CBasePlayer* Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		if (!Item.Disabled)
		{
			switch (Item.Info)
			{
				case 1:
				{
					this->AddSpawn(Player->edict()->v, UNASSIGNED, 15.0f);
					this->AddSpawnMenu(Player);
					break;
				}
				case 2:
				{
					this->AddSpawn(Player->edict()->v, TERRORIST, 15.0f);
					this->AddSpawnMenu(Player);
					break;
				}
				case 3:
				{
					this->AddSpawn(Player->edict()->v, CT, 15.0f);
					this->AddSpawnMenu(Player);
					break;
				}
				case 4:
				{
					this->Menu(Player);
					break;
				}
			}
		}
	}
}

void CPugSpawnEdit::DiscardChanges(CBasePlayer* Player)
{
	gPugMenu[Player->entindex()].Create(false, E_MENU::DM_SPAWN_EDIT_DISCARD, _T("Descartar todas as alterações e sair do editor?"));

	gPugMenu[Player->entindex()].AddItem(1, false, 0, _T("Não, continuar editando"));

	gPugMenu[Player->entindex()].AddItem(2, false, 0, _T("Sim, descartar tudo e sair do editor"));

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 ^3Descartar todas alterações e sair do editor?"), gPugCvar.m_Tag->string);

	gPugMenu[Player->entindex()].Show(Player);
}

void CPugSpawnEdit::DiscardChangesHandle(CBasePlayer* Player, P_MENU_ITEM Item)
{
	if (Player)
	{
		switch (Item.Info)
		{
			case 1:
			{
				this->Menu(Player);
				break;
			}
			case 2:
			{
				this->RemoveEntity(-1);

				Player->edict()->v.movetype = MOVETYPE_WALK;

                gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Todas as alterações foram descartadas."), gPugCvar.m_Tag->string);

				auto lpInfo = gPugClientCmd.Get(CMD_DM_SPAWN_EDITOR);

				if (lpInfo)
				{
					if (!lpInfo->Name.empty())
					{
						gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Use o comando ^3%s^1. para abrir o editor novamente."), gPugCvar.m_Tag->string, lpInfo->Name.c_str());
					}
				}

                
                break;
			}
		}
	}
}

edict_t* CPugSpawnEdit::MakeEntity(int EntityIndex)
{
	edict_t* pEntity = nullptr;

	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			pEntity = g_engfuncs.pfnCreateNamedEntity(this->m_InfoTarget);

			if (!FNullEnt(pEntity))
			{
				pEntity->v.classname = g_engfuncs.pfnAllocString("view_spawn");

				switch (Spawn.second.Team)
				{
					case 0:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");
						break;
					}
					case 1:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/terror/terror.mdl");
						break;
					}
					case 2:
					{
						g_engfuncs.pfnSetModel(pEntity, "models/player/urban/urban.mdl");
						break;
					}
				}

				pEntity->v.solid = SOLID_SLIDEBOX;
				pEntity->v.movetype = MOVETYPE_NOCLIP;
				pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
				pEntity->v.sequence = 1;

				if (this->m_Entities[Spawn.first])
				{
					g_engfuncs.pfnRemoveEntity(this->m_Entities[Spawn.first]);
				}

				this->m_Entities[Spawn.first] = pEntity;

				this->UnGlowEnt(Spawn.first);
			}
		}
	}
	else
	{
		pEntity = g_engfuncs.pfnCreateNamedEntity(this->m_InfoTarget);

		if (!FNullEnt(pEntity))
		{
			pEntity->v.classname = g_engfuncs.pfnAllocString("view_spawn");

			g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");

			switch (this->m_Spawns[EntityIndex].Team)
			{
				case 0:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/vip/vip.mdl");
					break;
				}
				case 1:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/terror/terror.mdl");
					break;
				}
				case 2:
				{
					g_engfuncs.pfnSetModel(pEntity, "models/player/urban/urban.mdl");
					break;
				}
			}

			pEntity->v.solid = SOLID_SLIDEBOX;
			pEntity->v.movetype = MOVETYPE_NOCLIP;
			pEntity->v.flags = (pEntity->v.flags & FL_ONGROUND);
			pEntity->v.sequence = 1;

			if (this->m_Entities[EntityIndex])
			{
				g_engfuncs.pfnRemoveEntity(this->m_Entities[EntityIndex]);
			}

			this->m_Entities[EntityIndex] = pEntity;

			this->UnGlowEnt(EntityIndex);
		}
	}

	return pEntity;
}

void CPugSpawnEdit::RemoveEntity(int EntityIndex)
{
	if (EntityIndex < 0)
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if (!FNullEnt(this->m_Entities[Spawn.first]))
			{
				g_engfuncs.pfnRemoveEntity(this->m_Entities[Spawn.first]);
			}
		}

		this->m_Spawns.clear();

		this->m_Entities.clear();

		this->m_Marked.clear();
	}
	else
	{
		if (!FNullEnt(this->m_Entities[EntityIndex]))
		{
			g_engfuncs.pfnRemoveEntity(this->m_Entities[EntityIndex]);
		}

		this->m_Spawns.erase(EntityIndex);

		this->m_Entities[EntityIndex] = nullptr;

		this->m_Marked[EntityIndex] = -1;
	}
}

void CPugSpawnEdit::GlowEnt(int EntityIndex, vec3_t Color)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxGlowShell;

		pEntity->v.renderamt = 127.0f;

		pEntity->v.rendermode = kRenderTransAlpha;

		pEntity->v.rendercolor = Color;
	}
}

void CPugSpawnEdit::UnGlowEnt(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		this->SetPosition(EntityIndex);

		pEntity->v.renderfx = kRenderFxNone;

		pEntity->v.renderamt = 255.0f;

		pEntity->v.rendermode = kRenderTransAlpha;
	}
}

bool CPugSpawnEdit::IsStuck(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

    if (!FNullEnt(pEntity))
    {
        TraceResult trResult = {};

        g_engfuncs.pfnTraceHull(pEntity->v.origin, pEntity->v.origin, dont_ignore_monsters, (pEntity->v.flags & FL_DUCKING) ? 3 : 1, pEntity, &trResult);

        return (trResult.fStartSolid || trResult.fAllSolid || !trResult.fInOpen);
    }

    return false;
}

void CPugSpawnEdit::SetPosition(int EntityIndex)
{
	auto pEntity = this->m_Entities[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		pEntity->v.origin = this->m_Spawns[EntityIndex].Vecs;

		pEntity->v.angles = this->m_Spawns[EntityIndex].Angles;

		pEntity->v.v_angle = this->m_Spawns[EntityIndex].VAngles;

		pEntity->v.fixangle = 1;
	}
}

void CPugSpawnEdit::AddSpawn(entvars_t v, int Team, float FixOrigin)
{
	P_SPAWN Info = {};

	if (FixOrigin != 0.0f)
	{
		v.origin[2] += FixOrigin;
	}

	Info.Vecs = v.origin;

	Info.Angles = v.angles;

	Info.Team = Team;

	Info.VAngles = v.v_angle;

	auto Index = this->m_Spawns.size();

	this->m_Spawns[Index] = Info;

	this->MakeEntity(Index);
}

void CPugSpawnEdit::EditSpawn(CBasePlayer *Player, entvars_t v, float FixOrigin)
{
	auto EntityIndex = Player->entindex();

	auto pEntity = this->m_Marked[EntityIndex];

	if (!FNullEnt(pEntity))
	{
		for (auto const& Spawn : this->m_Spawns)
		{
			if ((this->m_Spawns[EntityIndex].Vecs[0] == Spawn.second.Vecs[0]) && (this->m_Spawns[EntityIndex].Vecs[1] == Spawn.second.Vecs[1]) && ((this->m_Spawns[EntityIndex].Vecs[2] - Spawn.second.Vecs[2]) <= 15.0f))
			{
				this->RemoveEntity(this->m_Marked[EntityIndex]);

				if (FixOrigin != 0.0f)
				{
					v.origin[2] += FixOrigin;
				}

				this->m_Spawns[this->m_Marked[EntityIndex]].Vecs = v.origin;

				this->m_Spawns[this->m_Marked[EntityIndex]].Angles = v.angles;

				this->m_Spawns[this->m_Marked[EntityIndex]].Team = 0;

				this->m_Spawns[this->m_Marked[EntityIndex]].VAngles = v.v_angle;

				this->MakeEntity(this->m_Marked[EntityIndex]);

				this->GlowEnt(this->m_Marked[EntityIndex], Vector(255.0f, 0.0f, 0.0f));

				break;
			}
		}
	}
}

void CPugSpawnEdit::DeleteSpawn(int EntityIndex)
{
	this->UnGlowEnt(this->m_Marked[EntityIndex]);

	this->RemoveEntity(this->m_Marked[EntityIndex]);

	auto it = this->m_Spawns.find(this->m_Marked[EntityIndex]);

	if (it != this->m_Spawns.end())
	{
		this->m_Spawns.erase(it);
	}

	this->MakeEntity(-1);

	this->m_Marked[EntityIndex] = -1;
}

int CPugSpawnEdit::ClosestSpawn(edict_t* pEdict)
{
	auto Result = 0;

	if (!FNullEnt(pEdict))
	{
		auto LastDistance = 9999.0f;

		for (auto const& Spawn : this->m_Spawns)
		{
			auto Distance = (pEdict->v.origin - Spawn.second.Vecs).Length();

			if (Distance < LastDistance)
			{
				LastDistance = Distance;

				Result = Spawn.first;
			}
		}
	}

	return Result;
}

void CPugSpawnEdit::Refresh(CBasePlayer* Player)
{
	this->UnGlowEnt(this->m_Marked[Player->entindex()]);

	this->m_Marked[Player->entindex()] = this->ClosestSpawn(Player->edict());

	this->GlowEnt(this->m_Marked[Player->entindex()], Vector(255.0f, 200.0f, 20.0f));

	auto Spawn = this->m_Spawns.find(this->m_Marked[Player->entindex()]);

	if (Spawn != this->m_Spawns.end())
	{
		gPugUtil.PrintColor
		(
			Player->edict(),
			E_PRINT_TEAM::DEFAULT,
			_T("^4[%s]^1 Spawn mais próximo: Número ^3%d^1 -> Time ^3%s^1"),
			gPugCvar.m_Tag->string,
			Spawn->first,
			(Spawn->second.Team == UNASSIGNED) ? "ANY" : (Spawn->second.Team == CT ? "CT" : "TR")
		);

		gPugUtil.PrintColor
		(
			Player->edict(),
			E_PRINT_TEAM::DEFAULT,
			_T("^4[%s]^1 Origem [%.0f %.0f %.0f] Ângulo [%.0f %.0f %.0f] VÂngulo [%.0f %.0f %.0f]"),
			gPugCvar.m_Tag->string,
			Spawn->second.Vecs[0],
			Spawn->second.Vecs[1],
			Spawn->second.Vecs[2],
			Spawn->second.Angles[0],
			Spawn->second.Angles[1],
			Spawn->second.Angles[2],
			Spawn->second.VAngles[0],
			Spawn->second.VAngles[1],
			Spawn->second.VAngles[2]
		);

		if (this->IsStuck(this->m_Marked[Player->entindex()]))
		{
			gPugUtil.PrintColor
			(
				Player->edict(),
				E_PRINT_TEAM::RED,
				_T("^4[%s]^1 Esse Spawn (Número ^4%d^1) (Time ^4%s^1) pode estar preso: ^3Faça a correção."),
				gPugCvar.m_Tag->string,
				Spawn->first,
				(Spawn->second.Team == UNASSIGNED) ? "ANY" : (Spawn->second.Team == CT ? "CT" : "TR")
			);
		}
	}
}

void CPugSpawnEdit::ShowStats(CBasePlayer* Player)
{
	auto TotalSpawns = 0;
	auto TotalRandom = 0;
	auto TotalTR = 0;
	auto TotalCT = 0;

	for (auto const& Spawn : this->m_Spawns)
	{
		TotalSpawns++;

		switch (Spawn.second.Team)
		{
			case UNASSIGNED:
			{
				TotalRandom++;
				break;
			}
			case TERRORIST:
			{
				TotalTR++;
				break;
			}
			case CT:
			{
				TotalCT++;
				break;
			}
		}
	}

	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Spawns: %d; Aleatório: %d; TR: %d; CT: %d"), gPugCvar.m_Tag->string, TotalSpawns, TotalRandom, TotalTR, TotalCT);
	gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Posição Atual: X: %.3f Y: %.3f Z: %.3f"), gPugCvar.m_Tag->string, Player->edict()->v.origin.x, Player->edict()->v.origin.y, Player->edict()->v.origin.z);
}

void CPugSpawnEdit::Save(CBasePlayer* Player)
{
	if (!this->m_Spawns.empty())
	{
		char Path[MAX_PATH] = { 0 };

		snprintf(Path, sizeof(Path), "%s/%s/spawns/%s.cfg", gpMetaUtilFuncs->pfnGetGameInfo(PLID, GINFO_GAMEDIR), gPugUtil.GetPath(), STRING(gpGlobals->mapname));

		std::ofstream File(Path);

		if (File.is_open())
		{
			for (auto const& Spawn : this->m_Spawns)
			{
				File << "pug_add_spawn\t";
				File << static_cast<int>(Spawn.second.Vecs[0]) << "\t" << static_cast<int>(Spawn.second.Vecs[1]) << "\t" << static_cast<int>(Spawn.second.Vecs[2]) << "\t";
				File << static_cast<int>(Spawn.second.Angles[0]) << "\t" << static_cast<int>(Spawn.second.Angles[1]) << "\t" << static_cast<int>(Spawn.second.Angles[2]) << "\t";
				File << Spawn.second.Team << "\t";
				File << static_cast<int>(Spawn.second.VAngles[0]) << "\t" << static_cast<int>(Spawn.second.VAngles[1]) << "\t" << static_cast<int>(Spawn.second.VAngles[2]);
				File << "\n";
			}

			File.close();

			gPugDM.Load();
			
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 ^3%u^1 Spawns salvos no arquivo: ^3%s.cfg"), gPugCvar.m_Tag->string, this->m_Spawns.size(), STRING(gpGlobals->mapname));
		}
		else
		{
			gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::DEFAULT, _T("^4[%s]^1 Falha ao abrir: ^3%s.cfg"), gPugCvar.m_Tag->string, STRING(gpGlobals->mapname));
		}
	}
	else
	{
		gPugUtil.PrintColor(Player->edict(), E_PRINT_TEAM::RED, _T("^4[%s]^1 Falha ao salvar arquivo: ^3Nenhum Spawn na lista."), gPugCvar.m_Tag->string);
	}	
}
