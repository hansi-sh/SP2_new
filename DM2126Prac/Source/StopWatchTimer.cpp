#include "StopWatchTimer.h"



StopWatchTimer::StopWatchTimer()
{
	i_StartTime = 120;
	remainingtimePR;
}


StopWatchTimer::~StopWatchTimer()
{

}

int StopWatchTimer::i_GetTime()
{
	return i_StartTime;
}

void StopWatchTimer::v_SetTime(float time)
{
	i_StartTime = time;
}
void StopWatchTimer::v_UpdateTime(double dt)
{
	i_StartTime -= 1*dt ;

}