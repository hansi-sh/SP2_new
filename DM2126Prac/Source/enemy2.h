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
	Vector3 AI_Position;
	int randNum,PrevNum;
	double timer;
	float Z1,Z2,Y,X,walkingspeed,check,checkhop;

};

