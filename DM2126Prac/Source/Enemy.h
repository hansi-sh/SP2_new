#pragma once
#include "Mesh.h"
class Enemy
{
public:
	Enemy();
	~Enemy();
	Vector3 Enemymove(double dt);
private:
	float E_carSpeed, E_maxSpeed;
	Vector3 E_Position, E_Direction;
	float randomChoices;
};

