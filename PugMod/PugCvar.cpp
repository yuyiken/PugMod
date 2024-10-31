#include "precompiled.h"

CPugCvar gPugCvar;

void CPugCvar::ServerActivate()
{
	// sv_restart
	this->m_SvRestart = gPugUtil.CvarRegister("sv_restart", "0");

	// bot_deathmatch
	this->m_BotDeathmatch = gPugUtil.CvarRegister("bot_deathmatch", "0");

	// Log Tag
	this->m_LogTag = gPugUtil.CvarRegister("pug_log_tag", "PUG");

	// Mínimo de jogadores para iniciar uma partida
	this->m_PlayersMin = gPugUtil.CvarRegister("pug_players_min", "10");

	// Máximo de jogadores em jogo
	this->m_PlayersMax = gPugUtil.CvarRegister("pug_players_max", "10");

	// Total de rounds
	this->m_Rounds = gPugUtil.CvarRegister("pug_rounds", "30");

	// Total de rounds em overtime
	this->m_RoundsOT = gPugUtil.CvarRegister("pug_rounds_ot", "6");

	// Tipo do overtime: 0 Votação, 1 ogar Overtime, 2 Permitir empate, 3 Morte súbita
	this->m_OvertimeType = gPugUtil.CvarRegister("pug_ot_type", "0");

	// Modo de times: 0 Votação, 1 Capitães, 2 Misturar Times, 3 Nenhum, 3 Balancear Skill, 4 Trocar Times, 5 Round Faca
	this->m_TeamType = gPugUtil.CvarRegister("pug_team_type", "0");

	// Configuração incial
	this->m_Config[STATE_DEAD] = gPugUtil.CvarRegister("pug_cfg_main", "pugmod.cfg");

	// Aquecimento
	this->m_Config[STATE_WARMUP] = gPugUtil.CvarRegister("pug_cfg_warmup", "warmup.cfg");

	// Votação
	this->m_Config[STATE_START] = gPugUtil.CvarRegister("pug_cfg_start", "start.cfg");

	// Primeiro Tempo
	this->m_Config[STATE_FIRST_HALF] = gPugUtil.CvarRegister("pug_cfg_1st", "esl.cfg");

	// Intervalo
	this->m_Config[STATE_HALFTIME] = gPugUtil.CvarRegister("pug_cfg_halftime", "halftime.cfg");

	// Segundo Tempo
	this->m_Config[STATE_SECOND_HALF] = gPugUtil.CvarRegister("pug_cfg_2nd", "esl.cfg");

	// Overtime
	this->m_Config[STATE_OVERTIME] = gPugUtil.CvarRegister("pug_cfg_overtime", "esl-ot.cfg");

	// Fim
	this->m_Config[STATE_END] = gPugUtil.CvarRegister("pug_cfg_end", "end.cfg");
}