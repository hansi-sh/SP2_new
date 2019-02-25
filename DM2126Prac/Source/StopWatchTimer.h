#ifndef STOPWATCH_TIMER_H
#define STOPWATCH_TIMER_H

class StopWatchTimer
{
private:
	double d_StartTime;
	double d_ambulancetimer;
	double d_TotalTimeAfterAmbulance;
public:
	StopWatchTimer();
	~StopWatchTimer();
	double d_GetPuzzleSceneTime();
	double d_GetTotalTimeAfterAmbulance();
	void v_UpdateTime(double dt);
	void v_SetPuzzleSceneTime(float time);
};

#endif