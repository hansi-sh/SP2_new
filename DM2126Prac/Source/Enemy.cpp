#include "Enemy.h"
#include <stdlib.h> 


Enemy::Enemy()
{
	E_carSpeed = 0;
	E_maxSpeed = 30.0f;
	E_Position = (0, 0, 0);
	E_Direction = (0, 0, 1);
	srand(time(NULL));
	randomChoices = 0;
}


Enemy::~Enemy()
{
}

Vector3 Enemy::Enemymove(double dt)
{
	/*randomChoices = rand() % 4;
	randomChoices;*/
	if (E_carSpeed < E_maxSpeed)
	{
		E_carSpeed += (5 * dt);
	}
	else if (E_carSpeed == E_maxSpeed)
	{
		E_carSpeed = E_maxSpeed;
	}
	

	E_Position += E_carSpeed * E_Direction*dt;
	std:: cout << E_carSpeed << std::endl;
	return E_Position;
	
}
