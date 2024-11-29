#pragma once

enum E_TASK_ID
{
    TASK_CHANGE_STATE = 1,
    TASK_PUG_TIMER = 2,
    TASK_TEST_ID
};

typedef struct S_TASK
{
    float   Delay;
    float   Time;
    bool    Loop;
    int     Parameter;
} P_TASK, *LP_TASK;

class CPugTask
{
public:
    void ServerActivate();
    void ServerDeactivate();
    
    void Set(int Index, float Delay, bool Loop, int Parameter);
    void Remove(int Index);
    void StartFrame();

private:
    std::map<int, P_TASK> m_Data;
};

extern CPugTask gPugTask;