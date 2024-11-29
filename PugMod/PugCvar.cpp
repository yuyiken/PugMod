#include "precompiled.h"

CPugCvar gPugCvar;

void CPugCvar::ServerActivate()
{
	// Log Tag
	//
	// Padrão: "PUG"
	this->m_Tag = this->Register("pug_tag", "PUG");

	// Prefixo do comando via say (Jogador)
	//
	// Padrão: "."
	this->m_CmdPrefixPlayer = this->Register("pug_cmd_prefix_player", ".");

	// Prefixo do comando via say (Admin)
	//
	// Padrão: "!"
	this->m_CmdPrefixAdmin = this->Register("pug_cmd_prefix_admin", "!");

	// Mínimo de jogadores para iniciar uma partida
	//
	// Padrão: "10"
	this->m_PlayersMin = this->Register("pug_players_min", "10");

	// Máximo de jogadores em jogo
	//
	// Padrão: "10"
	this->m_PlayersMax = this->Register("pug_players_max", "10");

	// Total de rounds
	//
	// Padrão: "30"
	this->m_Rounds = this->Register("pug_rounds", "30");

	// Total de rounds em overtime
	//
	// Padrão: "6"
	this->m_RoundsOT = this->Register("pug_rounds_ot", "6");

	// Tipo do overtime
	// 
	// 0 Votação
	// 1 Jogar overtime
	// 2 Permitir empate
	// 3 Morte súbita
	//
	// Padrão: "0"
	this->m_OvertimeType = this->Register("pug_ot_type", "0");

	// Tempo em segundos para iniciar a partida no modo aquecimento
	// Contagem a partir do momento em que todos jogadores estiverem no jogo
	//
	// Padrão: "60.0"
	this->m_WarmupTimeLimit = this->Register("pug_warmup_time_limit", "60.0");

	// Modo de times
	// 
	// 0 Votação
	// 1 Capitães
	// 2 Misturar Times
	// 3 Nenhum
	// 4 Balancear Skill
	// 5 Trocar Times
	// 6 Round Faca
	//
	// Padrão: "0"
	this->m_TeamType = this->Register("pug_team_type", "0");

	// Restringir armas
	// Cada posição é um item (Ver enum ItemID para as posições)
	//
	// 0 Inativo
	// 1 Ativo
	//
	// Padrão: "000000000000000000000000000000000000000"
	this->m_RestrictItem = this->Register("pug_restrict_item", "000000000000000000000000000000000000000");

	// Ativar modo Deathmatch
	// Se inativo, será o modo aquecimento comum
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "1"
	this->m_DM_Enable = this->Register("pug_dm_enable", "1");

	// Ocultar kill feed de outros jogadores
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "0"
	this->m_DM_HideKillFeed = this->Register("pug_dm_hide_kill_feed", "0");

	// Indicador de acertos na tela
	// 
	// 0 Inativo
	// 1 Ativo
	//
	// Padrão: "1"
	this->m_DM_HitIndicator = this->Register("pug_dm_hit_indicator", "1");

	// Somente aceita acertos na cabeça
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "0"
	this->m_DM_HSOnlyMode = this->Register("pug_dm_hs_mode", "0");

	// Exibir Taxa de mortes / frags e taxa de HS no hud
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "1"
	this->m_DM_HudKDRatio = this->Register("pug_dm_hud_kd_ratio", "1");

	// Pisca a tela ao matar outro jogador
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
	//
	// Padrão: "2"
	this->m_DM_KillFade = this->Register("pug_dm_kill_fade", "2");

	// Recuperar vida após frag
    //
    // 0 Inativo
    //
	// Padrão: "15"
	this->m_DM_KillHP = this->Register("pug_dm_kill_hp", "15");

	// Recuperar vida após frag (HS)
    //
    // 0 Inativo
    //
	// Padrão: "40"
	this->m_DM_KillHPHS = this->Register("pug_dm_kill_hp_hs", "40");

	// Recuperar armadura após frag
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
	//
	// Padrão: "1"
	this->m_DM_KillRepairArmor = this->Register("pug_dm_kill_repair_armor ", "1");

	// Exibir HP recuperado após frag
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "1"
	this->m_DM_KillHealedMsg = this->Register("pug_dm_kill_hp_msg ", "1");

	// Ativar som ao realizar frag
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
	//
	// Padrão: "2"
	this->m_DM_KillSound = this->Register("pug_dm_kill_sound", "2");

	// Exibe frags no lugar do dinheiro
    //
    // 0 Inativo
    // 1 Ativo
	//
	// Padrão: "1"
	this->m_DM_MoneyFrag = this->Register("pug_dm_money_frag", "0");
}

cvar_t* CPugCvar::Register(const char* Name, const char* Value)
{
	cvar_t* Pointer = g_engfuncs.pfnCVarGetPointer(Name);

	if (!Pointer)
	{
		this->m_Data[Name].name = Name;

		this->m_Data[Name].string = strdup(Value);

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