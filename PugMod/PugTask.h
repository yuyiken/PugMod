#pragma once

enum E_TASK
{
    SET_STATE,
    VOTE_MAP_CHANGE,
    LEADER_MENU,
    VOTE_SWAP_TEAM,
    SEND_HUDMESSAGE,
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
    std::map<int, P_TASK> m_Task;
};

extern CPugTask gPugTask;