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

void CPugTask::Set(int Index, float Delay, bool Loop, int Parameter)
{
    if (Delay > 0.0f)
    {
        this->m_Data[Index] = {Delay, (gpGlobals->time + Delay), Loop, Parameter};
    }
}

void CPugTask::Remove(int Index)
{
    if (!this->m_Data.empty())
    {
        if (this->m_Data.find(Index) != this->m_Data.end())
        {
            this->m_Data[Index] = {-1.0f, -1.0f, false, 0};
        }
    }
}

void CPugTask::StartFrame()
{
    if (!this->m_Data.empty())
    {
        for (auto it = this->m_Data.begin(); it != this->m_Data.end(); it++)
        {
            if ((it->second.Delay > 0.0f) && (it->second.Time > 0.0f))
            {
                if  (gpGlobals->time >= it->second.Time)
                {
                    switch (it->first)
                    {
                        case TASK_CHANGE_STATE:
                        {
                            gPugMod.SetState(it->second.Parameter);
                            break;
                        }
                        case TASK_PUG_TIMER:
                        {
                            gPugTimer.Task();
                            break;
                        }
                    }

                    if (it->second.Loop)
                    {
                        it->second.Time = (gpGlobals->time + it->second.Delay);
                    }
                    else
                    {
                        this->Remove(it->first);
                    }
                }
            }
        }
    }
}