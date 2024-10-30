#pragma once

constexpr int STATE_DEAD = 0;
constexpr int STATE_WARMUP = 1;
constexpr int STATE_START = 2;
constexpr int STATE_FIRST_HALF = 3;
constexpr int STATE_HALFTIME = 4;
constexpr int STATE_SECOND_HALF = 5;
constexpr int STATE_OVERTIME = 6;
constexpr int STATE_END = 7;

const char PUG_MOD_STATES_STR[][32] =
{
	"Morto",
	"Aquecimento",
	"Iniciando",
	"Primeiro Tempo",
	"Intervalo",
	"Segundo Tempo",
	"Overtime",
	"Finalizado"
};

class CPugMod
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void ServerFrame();
	void SetState(int State);

	int m_State;

	int m_ScoreTR;
	int m_scoreCT;
};

extern CPugMod gPugMod;
