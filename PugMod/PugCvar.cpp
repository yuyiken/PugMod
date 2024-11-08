#include "precompiled.h"

CPugCvar gPugCvar;

void CPugCvar::ServerActivate()
{
	// sv_restart
	this->m_SvRestart = this->Register("sv_restart", "0");

	// Log Tag
	this->m_LogTag = this->Register("pug_log_tag", "PUG");

	// Mínimo de jogadores para iniciar uma partida
	this->m_PlayersMin = this->Register("pug_players_min", "10");

	// Máximo de jogadores em jogo
	this->m_PlayersMax = this->Register("pug_players_max", "10");

	// Total de rounds
	this->m_Rounds = this->Register("pug_rounds", "30");

	// Total de rounds em overtime
	this->m_RoundsOT = this->Register("pug_rounds_ot", "6");

	// Tipo do overtime: 0 Votação, 1 ogar Overtime, 2 Permitir empate, 3 Morte súbita
	this->m_OvertimeType = this->Register("pug_ot_type", "0");

	// Modo de times: 0 Votação, 1 Capitães, 2 Misturar Times, 3 Nenhum, 3 Balancear Skill, 4 Trocar Times, 5 Round Faca
	this->m_TeamType = this->Register("pug_team_type", "0");

	// Configuração incial
	this->m_Config[STATE_DEAD] = this->Register("pug_cfg_main", "pugmod.cfg");

	// Deathmatch
	this->m_Config[STATE_DEATHMATCH] = this->Register("pug_cfg_deathmatch", "deathmatch.cfg");

	// Ativar modo Deathmatch
	this->m_DM_Enable = this->Register("pug_dm_enable", "1");

	// Ocultar kill feed de outros jogadores
	this->m_DM_HideKillFeed = this->Register("pug_dm_hide_kill_feed", "0");

	// Indicador de acertos na tela: 0 Inativo, 1 Exibir ponto (*), 2 Indicador de mira (>   <), 3 Exibir dano, 4 Hitbox
	this->m_DM_HitIndicator = this->Register("pug_dm_hit_indicator", "3");

	// Somente aceita acertos na cabeça
	this->m_DM_HSOnlyMode = this->Register("pug_dm_hs_mode", "1");

	// Exibir Taxa de mortes / frags e taxa de HS no hud
	this->m_DM_HudKDRatio = this->Register("pug_dm_hud_kd_ratio", "1");

	// Pisca a tela ao matar outro jogador: 1 Ativar, 2 Somente no HS
	this->m_DM_KillFade = this->Register("pug_dm_kill_fade", "1");

	// Recuperar vida após frag
	this->m_DM_KillHP = this->Register("pug_dm_kill_hp", "15");

	// Recuperar vida após frag (HS)
	this->m_DM_KillHPHS = this->Register("pug_dm_kill_hp_hs", "40");

	// Recuperar armadura após frag: 0 Inativo, 1 Ativo, 2 Apenas no HS
	this->m_DM_KillRepairArmor = this->Register("pug_dm_kill_repair_armor ", "1");

	// Exibir HP recuperado após frag
	this->m_DM_KillHealedMsg = this->Register("pug_dm_kill_hp_msg ", "1");

	// Ativar som ao realizar frag: 0 Inativo, 1 Ativo, 2 Apenas HS
	this->m_DM_KillSound = this->Register("pug_dm_kill_sound", "2");

	// Exibe frags no lugar do dinheiro
	this->m_DM_MoneyFrag = this->Register("csdm_money_as_frags", "1");
}

cvar_t* CPugCvar::Register(const char* Name, const char* Value)
{
	cvar_t* Pointer = g_engfuncs.pfnCVarGetPointer(Name);

	if (!Pointer)
	{
		this->m_Data[Name].name = Name;

		this->m_Data[Name].string = "";

		this->m_Data[Name].flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

		g_engfuncs.pfnCVarRegister(&this->m_Data[Name]);

		Pointer = g_engfuncs.pfnCVarGetPointer(this->m_Data[Name].name);

		if (Pointer)
		{
			g_engfuncs.pfnCvar_DirectSet(Pointer, Value);
		}
	}

	return Pointer;
}