
#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::ServerActivate()
{
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
}

void CPugMod::ServerDeactivate()
{
	
}
