#pragma once

enum E_PUG_STATE
{
    STATE_DEAD,
    STATE_DEATHMATCH,
    STATE_VOTEMAP,
    STATE_VOTETEAM,
    STATE_CAPTAIN,
    STATE_KNIFE_ROUND,
    STATE_FIRST_HALF,
    STATE_HALFTIME,
    STATE_SECOND_HALF,
    STATE_OVERTIME,
    STATE_END
};

constexpr std::array<const char *, STATE_END + 1> g_Pug_Config =
{
    "pugmod", 
    "deathmatch", 
    "votemap", 
    "voteteam", 
    "captain", 
    "kniferound", 
    "esl", 
    "halftime", 
    "esl", 
    "esl-ot", 
    "end"
};

constexpr std::array<const char *, STATE_END + 1> g_Pug_String =
{
    "Morto", 
    "Deathmatch", 
    "Escolha do Mapa", 
    "Escolha do Time", 
    "Capitães", 
    "Round Faca", 
    "Primeiro Tempo", 
    "Intervalo", 
    "Segundo Tempo", 
    "Overtime", 
    "Fim"
};

constexpr std::array<const char *, SPECTATOR + 1> g_Pug_TeamId =
{
    "Unnasigned",
    "Terroristas",
    "Contra-Terroristas",
    "Espectadores"
};

constexpr std::array<hudtextparms_t, 2> g_Pug_HudParam =
{{
    {0.01f, 0.18f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 12.0f, 2.0f, 1},
    {0.01f, 0.18f, 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 12.0f, 2.0f, 2},
}};

class CPugMod
{
public:
    void ServerActivate();
    void ServerDeactivate();

    int GetState();
    int SetState(int State);
    void NextState(float Delay);
    void SwapTeams();

    std::array<int, SPECTATOR> GetScore();
    int GetRound();
    int GetWinner();
    bool LastRound(int State);

    bool ChooseTeam(CBasePlayer *Player, int Slot);
    void GetIntoGame(CBasePlayer *Player);
    bool HasRestrictItem(CBasePlayer *Player, ItemID Item, ItemRestType Type);
    void DropClient(edict_t *pEntity);

    void RestartRound();
    void RoundStart();
    void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

    void Status(CBasePlayer *Player);
    void Scores(CBasePlayer *Player);

    void SendHudMessage();

private:
    int m_State = STATE_DEAD;
    std::array<std::array<int, SPECTATOR + 1>, STATE_END + 1> m_Score = {};
    float m_NextFrame = 0.0f;
};

extern CPugMod gPugMod;