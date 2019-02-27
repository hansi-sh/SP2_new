#ifndef STOPWATCH_TIMER_H
#define STOPWATCH_TIMER_H

class StopWatchTimer
{
private:
	double d_timer;
public:
	StopWatchTimer();
	~StopWatchTimer();
	void v_SetPuzzleSceneTime(float time);
	double d_GetPuzzleSceneTime();
	void v_SetAmbulanceTime(float time);
	double d_GetAmbulanceTimer();
	void v_SetRaceSceneTime(float time);
	double d_GetRaceSceneTime();
	void v_UpdateTime(double dt);
};

#endif