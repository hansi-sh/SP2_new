#include "Enemy.h"
#include <stdlib.h> 


Enemy::Enemy()
{
	E_carSpeed = 0;
	E_Acceleration = 10.0f;
	E_Position = Vector3(0, 0, 0);
	E_Direction = Vector3(0, 0, 1);
	srand(time(NULL));
	randomChoices = 0;
}


Enemy::~Enemy()
{
}

void Enemy::SetEnemyPosition(Vector3 pos)
{
	this->E_Position = pos;
}

void Enemy::E_carspeed(bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt)
{
	if (stepAccelerator)
		E_carSpeed += float(E_Acceleration * dt);
	//Increase f_CarSpeed in terms of acceleration and time (v=u+at)
	else if (stepBrakes)
		E_carSpeed -= float(E_Acceleration * dt);
	//Decrease f_CarSpeed in terms of acceleration and time (v=u+at)

	if (steerWheel)//if rotate
	{
		//Decrease f_CarSpeed when rotating
		if (E_carSpeed > 0.0)
			E_carSpeed = E_carSpeed - 0.05f; //setspeed=getspeed-0.05
		else
			E_carSpeed = E_carSpeed + 0.05f;
	}

	//Deceleration always trys to slow car down
	E_carSpeed -= E_carSpeed * 0.2 * (dt);
}

void Enemy::v_UpdateEnemyCarDirection(float Degree, float previousDegree)
{
	Mtx44 rotateBy;
	rotateBy.SetToRotation((Degree - previousDegree), 0, 1, 0);

	E_Direction = (rotateBy * E_Direction).Normalize();
}

float Enemy::f_GetEnemySpeed()
{
	return E_carSpeed;
}

void Enemy::v_SetEnemySpeed(float newSpeed)
{
	E_carSpeed = newSpeed;
}

Vector3 Enemy::V_UpdateenemyCarPos(double dt)
{
	E_Position += E_carSpeed * E_Direction * dt;
	if (E_Position.z>230)
	{
		E_Position.z = -200;
	}
	return E_Position;
}

void Enemy::enemyMove(bool stepAccelerator, bool stepBrakes)
{
	
}

