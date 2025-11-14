#pragma once

enum E_TASK
{
    SET_STATE,
    MAP_CHANGE,
    LEADER_MENU,
    VOTE_SWAP_TEAM,
    VOTE_OVERTIME,
    ROUND_MESSAGE,
    ROUND_END_STATS,
    PAUSE_MATCH
};

typedef struct S_TASK
{
    float Delay;
    float Time;
    bool Loop;
    int Parameter;
} P_TASK, *LP_TASK;

class CPugTask
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void Create(int Index, float Delay, bool Loop, int Parameter);
    void Remove(int Index);
    void Execute(int Index);
    void StartFrame();

private:
    std::map<int, P_TASK> m_Task = {};
};

extern CPugTask gPugTask;