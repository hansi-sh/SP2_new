#include "StopWatchTimer.h"



StopWatchTimer::StopWatchTimer()
{
	d_StartTime = 12 ;
	d_ambulancetimer = 30;
	d_RaceTimer = 0;
	d_TotalTimeAfterAmbulance = 0;
	d_TimeLeftAfterPuzzle;
}


StopWatchTimer::~StopWatchTimer()
{

}

double StopWatchTimer::d_GetPuzzleSceneTime()
{
	return d_StartTime;
}
double StopWatchTimer::d_GetAmbulanceTimer()
{
	return d_ambulancetimer;
}
double StopWatchTimer::d_GetRaceSceneTime()
{
	return d_RaceTimer;
}
double StopWatchTimer::d_GetTimeLeftAfterPuzzle()
{
	return d_TimeLeftAfterPuzzle;
}
void StopWatchTimer::v_SetPuzzleSceneTime(float time)
{
	d_StartTime = time;
}
void StopWatchTimer::v_SetRaceSceneTime(float time) 
{
	d_RaceTimer = time;
}
void StopWatchTimer::v_UpdateTime(double dt)
{
	d_StartTime -= 1 * dt ;
	d_ambulancetimer -= 1 * dt;
	d_RaceTimer -= 1 * dt;
}
