#pragma once

enum E_PUG_STATE
{
    STATE_DEAD,
    STATE_WARMUP,
    STATE_DEATHMATCH,
    STATE_VOTEMAP ,
    STATE_VOTETEAM ,
    STATE_CAPTAIN ,
    STATE_KNIFE_ROUND,
    STATE_FIRST_HALF,
    STATE_HALFTIME,
    STATE_SECOND_HALF,
    STATE_OVERTIME,
    STATE_END
};

typedef struct S_PUG_STATE
{
    E_PUG_STATE Id;
    const char* Name;
    const char* Config;
} P_PUG_STATE, *LP_PUG_STATE;
class CPugMod
{
public:
    void ServerActivate();
    void ServerDeactivate();

    int GetState();
    int SetState(int State);
    
    void Status(edict_t *pEntity);

private:
    int m_State = STATE_DEAD;

    const std::array<P_PUG_STATE, STATE_END + 1U> m_Data
    {{
        {STATE_DEAD, "Morto", "pugmod"},
        {STATE_WARMUP, "Aquecimento", "warmup"},
        {STATE_DEATHMATCH, "Deathmatch", "deathmatch"},
        {STATE_VOTEMAP, "Escolha do Mapa", "votemap"},
        {STATE_VOTETEAM, "Escolha do Time", "voteteam"},
        {STATE_CAPTAIN, "Escolha de Capitão", "captains"},
        {STATE_KNIFE_ROUND, "Round Faca", "kniferound"},
        {STATE_FIRST_HALF, "Primeiro Tempo", "esl"},
        {STATE_HALFTIME, "Intervalo", "halftime"},
        {STATE_SECOND_HALF, "Segundo Tempo", "esl"},
        {STATE_OVERTIME, "Overtime", "esl-ot"},
        {STATE_END, "Fim", "end"}
    }};
};

extern CPugMod gPugMod;