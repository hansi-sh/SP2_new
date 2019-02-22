#ifndef STOPWATCH_TIMER_H
#define STOPWATCH_TIMER_H

class StopWatchTimer
{
private:
	double i_StartTime;
	float f_UpdatedTime;
	float remainingtimePR;
public:
	StopWatchTimer();
	~StopWatchTimer();
	int i_GetTime();
	void v_UpdateTime(double dt);
	void v_SetTime(float time);
};

#endif