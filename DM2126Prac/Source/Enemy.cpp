#include "Enemy.h"
#include <stdlib.h> 


Enemy::Enemy()
{
	E_carSpeed = 0;
	randch = false;
	E_rotateAmt = 0;
	E_Rotate = 0;
	E_Acceleration = 10.0f;
	E_Position = Vector3(0, 0, 0);
	E_Direction = Vector3(0, 0, 1);
	srand(time(NULL));
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

float Enemy::v_UpdateEnemyCarDirection(float Degree)
{
	float rad = Math::DegreeToRadian(Degree);
	E_Direction = Vector3(sin(rad), 0, cos(rad));
	return Degree;
}

float Enemy::f_GetEnemySpeed()
{
	return E_carSpeed;
}

float Enemy::getenemyrotate()
{
	return E_Rotate;
}

void Enemy::v_SetEnemySpeed(float newSpeed)
{
	E_carSpeed = newSpeed;
}

float Enemy::enemyrotatesmount()
{
	if (fabs(E_carSpeed) < 3.0f)
	{
		E_rotateAmt = 0.0f;
	}
	else if (fabs(E_carSpeed) < 20.0f)
	{
		E_rotateAmt = 0.3f;

	}
	else if (fabs(E_carSpeed) < 40.0f)
	{
		E_rotateAmt = 0.5f;

	}
	else if (fabs(E_carSpeed) < 60.0f)
	{
		E_rotateAmt = 0.1f;
	}
	return E_rotateAmt;
}

int Enemy::randchecker(bool randomise, int random)
{
	if (randomise == true)
	{
		return random;
	}
	else
	{
		random = rand() % 4 + 2;
		randomise = true;
		return random;
	}
}

Vector3 Enemy::V_UpdateenemyCarPos(double dt)
{
	E_Position += E_carSpeed * E_Direction * dt;
	return E_Position;
}

Vector3 Enemy::enemyMove(Vector3 player, bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt, float RotateEnemyBody, int random, bool randcheck)
{
	if ((player.z - E_Position.z > 0) && (E_Acceleration < 22))//checking in front
	{
		steerWheel = false;
		E_Acceleration += 5.0f;
		E_Rotate = RotateEnemyBody = 0;
	}
	else if ((player.z - E_Position.z < -5.0f))
	{
		E_Acceleration = 10.0f;
		if (random == 1)
		{
			steerWheel = false;
			//random form 2 to 5
			randchecker(randcheck, random);
		}
		if ((random == 2))
		{
			if ((E_Position.x > 10.0f))
			{
				if ((E_Rotate > -10.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//right
					E_Rotate = RotateEnemyBody;
				}
			}
			else if ((E_Position.x < 1.0f) && (E_Position.x >= 0.0f))
			{
				if ((E_Rotate < 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//left
					E_Rotate = RotateEnemyBody;
				}
				steerWheel = false;
			}
			randcheck = false;
			randchecker(randcheck, random);
		}
		
		if ((random == 3))
		{
			if ((E_Position.x > 10.0f))
			{
				if ((E_Rotate > -10.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//right
					E_Rotate = RotateEnemyBody;
				}
			}
			else if ((E_Position.x < -14.0f) && (E_Position.x >= -16.0f))
			{
				if ((E_Rotate < 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//left
					E_Rotate = RotateEnemyBody;
				}
				steerWheel = false;
				randcheck = false;
				randchecker(randcheck, random);
			}
		}
		if ((random == 4))
		{
			if ((E_Position.x < -10.0f))
			{
				if ((E_Rotate < 10.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//right
					E_Rotate = RotateEnemyBody;
				}
			}
			else if ((E_Position.x < 0.0f) && (E_Position.x >= -1.0f))
			{
				if ((E_Rotate >0.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//left
					E_Rotate = RotateEnemyBody;
				}
				steerWheel = false;
				randcheck = false;
				randchecker(randcheck, random);
			}
			
		}
		if ((random == 5))
		{
			if ((E_Position.x < 10.0f))
			{
				if ((E_Rotate < 10.0f))
				{
					steerWheel = true;
					RotateEnemyBody += enemyrotatesmount();//right
					E_Rotate = RotateEnemyBody;
				}
			}
			else if ((E_Position.x > 13.0f) && (E_Position.x <= 16.0f))
			{
				if ((E_Rotate > 0.0f))
				{
					steerWheel = true;
					RotateEnemyBody -= enemyrotatesmount();//left
					E_Rotate = RotateEnemyBody;
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
	return E_Position;
}

Vector3 Enemy::collidedwithPlayer(Vector3 player)
{

	return Vector3();
}

