#include "precompiled.h"

CPugTask gPugTask;

void CPugTask::ServerActivate()
{
	this->m_Data.clear();
}

void CPugTask::ServerDeactivate()
{
	this->m_Data.clear();
}

void CPugTask::Create(int Index, float Delay, bool Repeat, int Parameter)
{
	if (this->m_Data.find(Index) == this->m_Data.end())
	{
		this->m_Data[Index].Index = Index;
		this->m_Data[Index].NextTime = (gpGlobals->time + Delay);
		this->m_Data[Index].Repeat = Repeat;
		this->m_Data[Index].Parameter = Parameter;
	}
}

void CPugTask::Remove(int Index)
{
	if (this->m_Data.find(Index) != this->m_Data.end())
	{
		this->m_Data.erase(Index);
	}
}

void CPugTask::ServerFrame()
{
	if (!this->m_Data.empty())
	{
		for (auto it = this->m_Data.begin(); it != this->m_Data.end(); it++)
		{
			if (it->second.NextTime > 0.0f)
			{
				if (gpGlobals->time >= it->second.NextTime)
				{
					switch (it->second.Index)
					{
						case TASK_CHANGE_STATE:
						{
							gPugMod.SetState(it->second.Parameter);
							break;
						}
					}

					this->Remove(it->second.Index);
				}
			}
		}
	}
}