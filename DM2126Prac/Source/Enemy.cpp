#include "Enemy.h"
#include <stdlib.h> 

///dasdas
Enemy::Enemy()
{
	F_EnemyCarSpeed = 0;
	F_EnemyRotateAmt = 0;
	F_EnemyRotate = 0;
	F_EnemyAcceleration = 10.0f;
	V_EnemyPosition = Vector3(0, 0, 0);
	V_EnemyDirection = Vector3(0, 0, 1);
	srand(time(NULL));
}


Enemy::~Enemy()
{
}

void Enemy::SetEnemyPosition(Vector3 pos)
{
	this->V_EnemyPosition = pos;
}

void Enemy::E_carspeed(bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt)
{
	if (stepAccelerator)
		F_EnemyCarSpeed += float(F_EnemyAcceleration * dt);
	//Increase f_CarSpeed in terms of acceleration and time (v=u+at)
	else if (stepBrakes)
		F_EnemyCarSpeed -= float(F_EnemyAcceleration * dt);
	//Decrease f_CarSpeed in terms of acceleration and time (v=u+at)

	if (steerWheel)//if rotate
	{
		//Decrease f_CarSpeed when rotating
		if (F_EnemyCarSpeed > 0.0)
			F_EnemyCarSpeed = F_EnemyCarSpeed - 0.05f; //setspeed=getspeed-0.05
		else
			F_EnemyCarSpeed = F_EnemyCarSpeed + 0.05f;
	}

	//Deceleration always trys to slow car down
	F_EnemyCarSpeed -= F_EnemyCarSpeed * 0.2 * (dt);
}

float Enemy::v_UpdateEnemyCarDirection(float Degree)
{
	float rad = Math::DegreeToRadian(Degree);
	V_EnemyDirection = Vector3(sin(rad), 0, cos(rad));
	return Degree;
}

float Enemy::f_GetEnemySpeed()
{
	return F_EnemyCarSpeed;
}

float Enemy::getenemyrotate()
{
	return F_EnemyRotate;
}

void Enemy::v_SetEnemySpeed(float newSpeed)
{
	F_EnemyCarSpeed = newSpeed;
}

float Enemy::enemyrotatesmount()
{
	if (fabs(F_EnemyCarSpeed) < 3.0f)
	{
		F_EnemyRotateAmt = 0.0f;
	}
	else if (fabs(F_EnemyCarSpeed) < 20.0f)
	{
		F_EnemyRotateAmt = 0.3f;

	}
	else if (fabs(F_EnemyCarSpeed) < 40.0f)
	{
		F_EnemyRotateAmt = 0.5f;

	}
	else if (fabs(F_EnemyCarSpeed) < 60.0f)
	{
		F_EnemyRotateAmt = 0.1f;
	}
	return F_EnemyRotateAmt;
}

int Enemy::randchecker(bool randomise, int random)
{
	if (randomise == true)
	{
		return random;
	}
	else
	{
		random = rand() % 3 + 2;
		randomise = true;
		return random;
	}
}

Vector3 Enemy::V_UpdateenemyCarPos(double dt)
{
	V_EnemyPosition += F_EnemyCarSpeed * V_EnemyDirection * dt;
	return V_EnemyPosition;
}

Vector3 Enemy::enemyMove(Vector3 player, bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt, float RotateEnemyBody, int random, bool randcheck)
{
	if ((player.z - V_EnemyPosition.z > 0) && (F_EnemyAcceleration < 22))//checking in front
	{
		steerWheel = false;
		F_EnemyAcceleration += 5.0f;
		F_EnemyRotate = RotateEnemyBody = 0;
	}
	else if ((player.z - V_EnemyPosition.z < -5.0f))
	{
		F_EnemyAcceleration = 10.0f;
		if (random == 1)
		{
			steerWheel = false;
			//random form 2 to 5
			randchecker(randcheck, random);
		}
		if ((random == 2))
		{
			if ((V_EnemyPosition.x > 10.0f))
			{
				if ((F_EnemyRotate > -10.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//right
					F_EnemyRotate = RotateEnemyBody;
				}
			}
			else if ((V_EnemyPosition.x < 1.0f) && (V_EnemyPosition.x >= 0.0f))
			{
				if ((F_EnemyRotate < 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//left
					F_EnemyRotate = RotateEnemyBody;
				}
				steerWheel = false;
			}
			randcheck = false;
			randchecker(randcheck, random);
		}
		
		if ((random == 3))
		{
			if ((V_EnemyPosition.x > 10.0f))
			{
				if ((F_EnemyRotate > -10.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//right
					F_EnemyRotate = RotateEnemyBody;
				}
			}
			else if ((V_EnemyPosition.x < -14.0f) && (V_EnemyPosition.x >= -16.0f))
			{
				if ((F_EnemyRotate < 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//left
					F_EnemyRotate = RotateEnemyBody;
				}
				steerWheel = false;
				randcheck = false;
				randchecker(randcheck, random);
			}
		}
		if ((random == 4))
		{
			if ((V_EnemyPosition.x < -10.0f))
			{
				if ((F_EnemyRotate < 10.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//right
					F_EnemyRotate = RotateEnemyBody;
				}
			}
			else if ((V_EnemyPosition.x < 0.0f) && (V_EnemyPosition.x >= -1.0f))
			{
				if ((F_EnemyRotate >0.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//left
					F_EnemyRotate = RotateEnemyBody;
				}
				steerWheel = false;
				randcheck = false;
				randchecker(randcheck, random);
			}
			
		}
		if ((random == 5))
		{
			if ((V_EnemyPosition.x < 10.0f))
			{
				if ((F_EnemyRotate < 10.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//right
					F_EnemyRotate = RotateEnemyBody;
				}
			}
			else if ((V_EnemyPosition.x > 13.0f) && (V_EnemyPosition.x <= 16.0f))
			{
				if ((F_EnemyRotate > 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//left
					F_EnemyRotate = RotateEnemyBody;
				}
				steerWheel = false;
				randcheck = false;
				randchecker(randcheck, random);
			}
		}
	}
	v_UpdateEnemyCarDirection(RotateEnemyBody);
	E_carspeed(stepAccelerator, stepBrakes, steerWheel, dt);
	V_UpdateenemyCarPos(dt);
	return V_EnemyPosition;
}


