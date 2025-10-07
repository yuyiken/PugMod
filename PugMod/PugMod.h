#pragma once

enum PUG_STATE
{
    STATE_DEAD = 0,
    STATE_DEATHMATCH = 1,
    STATE_VOTEMAP = 2,
    STATE_VOTETEAM = 3,
    STATE_CAPTAIN = 4,
    STATE_KNIFE_ROUND = 5,
    STATE_FIRST_HALF = 6,
    STATE_HALFTIME = 7,
    STATE_SECOND_HALF = 8,
    STATE_OVERTIME = 9,
    STATE_END = 10
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
    "Nenhum",
    "Terroristas",
    "Contra-Terroristas",
    "Espectadores"
};

constexpr std::array<const char *, SPECTATOR + 1> g_Pug_TeamShort =
{
    "NONE",
    "TR",
    "CT",
    "SPEC"
};

constexpr hudtextparms_t g_Pug_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 12.0f, 2.0f, 1};

class CPugMod
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void AddMap(const char* Map);
    std::map<int, std::string> GetMaps();

    int GetState();
    int SetState(int State);

    void SwapTeams();

    int GetRound();
    int GetWinner();
    std::array<int, SPECTATOR + 1> GetScore();

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

    static bool TeamScore(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity);
    static bool ScoreInfo(int msg_dest, int msg_type, const float* pOrigin, edict_t* pEntity);

private:
    int m_State = STATE_DEAD;
    std::array<std::array<int, STATE_END + 1>, SPECTATOR + 1> m_Score;
    std::array<int, SPECTATOR + 1> m_ScoreOT;
    std::map<int, std::string> m_MapList;
};

extern CPugMod gPugMod;