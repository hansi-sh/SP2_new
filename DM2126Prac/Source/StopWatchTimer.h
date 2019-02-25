#ifndef STOPWATCH_TIMER_H
#define STOPWATCH_TIMER_H

class StopWatchTimer
{
private:
	double d_StartTime;
	double d_ambulancetimer;
	double d_RaceTimer;
	double d_TotalTimeAfterAmbulance;
	double d_TimeLeftAfterPuzzle;
public:
	StopWatchTimer();
	~StopWatchTimer();
	double d_GetAmbulanceTimer();
	double d_GetPuzzleSceneTime();
	double d_GetRaceSceneTime();
	double d_GetTimeLeftAfterPuzzle();
	void v_UpdateTime(double dt);
	void v_SetPuzzleSceneTime(float time);
	void v_SetRaceSceneTime(float time);
};

#endif