#pragma once

typedef struct S_TASK
{
	int Index;
	float NextTime;
	bool Repeat;
} P_TASK, *LP_TASK;

class CPugTask
{
public:
	void ServerActivate();
	void ServerDeactivate();
	void Create(int Index, float Delay, bool Repeat);
	void Remove(int Index);
	void ServerFrame();

private:
	std::map<int, P_TASK> m_Data;
};

extern CPugTask gPugTask;