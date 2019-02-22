#include "StopWatchTimer.h"



StopWatchTimer::StopWatchTimer()
{
	d_StartTime = 120;
	d_ambulancetimer = 30;
	d_TotalTimeAfterAmbulance = 0;
}


StopWatchTimer::~StopWatchTimer()
{

}

double StopWatchTimer::d_GetPuzzleSceneTime()
{
	return d_StartTime;
}
double StopWatchTimer::d_GetTotalTimeAfterAmbulance()
{
	return d_ambulancetimer;
}
void StopWatchTimer::v_SetPuzzleSceneTime(float time)
{
	d_StartTime = time;
}
void StopWatchTimer::v_UpdateTime(double dt)
{
	d_StartTime -= 1*dt ;

}