#pragma once
#include "Vector3.h"
#include "Mtx44.h"
class enemy2
{
public:
	enemy2();
	~enemy2();
	Vector3 walking(float PrevX);
private:
	Vector3 AI_Position;
	float X1,X2,Y,Z;

};

