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
	float v_UpdateEnemyCarDirection(float Degree);
	float f_GetEnemySpeed();
	float getenemyrotate();
	void v_SetEnemySpeed(float newSpeed);
	float enemyrotatesmount();
	int randchecker(bool randomise,int random);
	Vector3 V_UpdateenemyCarPos(double dt);
	Vector3 enemyMove(Vector3 player ,bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt, float RotateEnemyBody,int random,bool randcheck);
private:
	float F_EnemyCarSpeed, F_EnemyAcceleration, F_EnemyRotateAmt, F_EnemyRotate;
	Vector3 V_EnemyPosition, V_EnemyDirection;
};

