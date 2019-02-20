#pragma once
#include "Vector3.h"
#include "Mtx44.h"
class Enemy
{
public:
	Enemy();
	~Enemy();
	void SetEnemyPosition(Vector3 pos);
	void E_carspeed(bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt);
	void v_UpdateEnemyCarDirection(float Degree, float previousDegree);
	float f_GetEnemySpeed();
	void v_SetEnemySpeed(float newSpeed);
	Vector3 V_UpdateenemyCarPos(double dt);
	void enemyMove(bool stepAccelerator, bool stepBrakes);
private:
	float E_carSpeed, E_Acceleration;
	Vector3 E_Position, E_Direction;
	float randomChoices;
};

