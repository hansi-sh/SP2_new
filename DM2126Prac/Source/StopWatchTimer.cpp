#include "StopWatchTimer.h"



StopWatchTimer::StopWatchTimer()
{
	d_timer = 0;
}


StopWatchTimer::~StopWatchTimer()
{

}

void StopWatchTimer::v_SetPuzzleSceneTime(float time)
{
	d_timer = time;
}
double StopWatchTimer::d_GetPuzzleSceneTime()
{
	return 	d_timer;
}
void StopWatchTimer::v_SetAmbulanceTime(float time)
{
	d_timer = time;
}
double StopWatchTimer::d_GetAmbulanceTimer()
{
	return 	d_timer;
}
void StopWatchTimer::v_SetRaceSceneTime(float time) 
{
	d_timer = time;
}
double StopWatchTimer::d_GetRaceSceneTime()
{
	return d_timer;
}
void StopWatchTimer::v_UpdateTime(double dt)
{
	d_timer -= dt;
}
