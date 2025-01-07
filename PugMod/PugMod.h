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

constexpr std::array<const char *, STATE_END + 1> g_Pug_Config = {"pugmod", "deathmatch", "votemap", "voteteam", "captain", "kniferound", "esl", "halftime", "esl", "esl-ot", "end"};
constexpr std::array<const char *, STATE_END + 1> g_Pug_String = {"Morto", "Deathmatch", "Escolha do Mapa", "Escolha do Time", "Capitães", "Round Faca", "Primeiro Tempo", "Intervalo", "Segundo Tempo", "Overtime", "Fim"};
constexpr std::array<const char *, SPECTATOR + 1> g_Pug_TeamId = {"Unnasigned", "Terroristas", "Contra-Terroristas", "Espectadores"};

class CPugMod
{
public:
    void ServerActivate();
    void ServerDeactivate();

    int GetState();
    int SetState(int State);

    void NextState();

    int GetRound();
    std::array<int, SPECTATOR> GetScore();

    bool ChooseTeam(CBasePlayer *Player, int Slot);
    void GetIntoGame(CBasePlayer *Player);
    bool HasRestrictItem(CBasePlayer *Player, ItemID Item, ItemRestType Type);
    void DropClient(edict_t *pEntity);

    void RestartRound();
    void RoundStart();
    void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

    void Status(CBasePlayer *Player);
    void Scores(CBasePlayer *Player);

private:
    int m_State = STATE_DEAD;
    std::array<std::array<int, SPECTATOR + 1>, STATE_END + 1> m_Score = {};
};

extern CPugMod gPugMod;