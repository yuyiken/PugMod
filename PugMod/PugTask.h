#pragma once

constexpr auto TASK_CHANGE_STATE = 1;

typedef struct S_TASK
{
	int Index;
	float NextTime;
	bool Repeat;
	int Parameter;
} P_TASK, *LP_TASK;

class CPugTask
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void Create(int Index, float Delay, bool Repeat, int Parameter);
	void Remove(int Index);
	void ServerFrame();

private:
	std::map<int, P_TASK> m_Data;
};

extern CPugTask gPugTask;