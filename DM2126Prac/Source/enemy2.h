#pragma once
#include "Vector3.h"
#include "Mtx44.h"
class enemy2
{
public:
	enemy2();
	~enemy2();
	Vector3 walking(float X,float Y,float PrevZ,double dt,bool checkmove,int choice,int velo);
	Vector3 setpos(float X, float Y, float Z);
	int choicemaker(int choice);
	float speed(int velo ,float dt);
private:
	Vector3 V_AI_Position;
	int I_randNum,I_PrevNum;
	double D_timer;
	float F_Z1,F_Z2,F_walkingspeed,F_check,F_checkhop;

};

