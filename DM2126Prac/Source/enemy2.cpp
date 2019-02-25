#include "enemy2.h"
#include <stdlib.h> 


enemy2::enemy2()
{
	timer = 0;
	check = 0;
	checkhop = 0;
	AI_Position = Vector3(0, 0, 0);
	Z1 = 0;//currentX
	Z2 = 0;//prevX
	Y = 0;
	X = 0;
	walkingspeed = 0;
}


enemy2::~enemy2()
{
}

float enemy2::speed(int velo,float dt)
{
	walkingspeed = Z2;
	if (AI_Position.z!=1450&&check ==0)
	{
		walkingspeed += float(velo * dt);
		if (AI_Position.z >= 1450)
		{
			check = 1;
		}
		return walkingspeed;
	}
	 if(AI_Position.z != -1350&&check==1)
	{
		walkingspeed -= float(velo * dt);
		if (AI_Position.z <= -1350)
		{
			check = 0;
		}
		return walkingspeed;
	}
	
}

Vector3 enemy2::walking(float X, float Y, float PrevZ, double dt, bool checkmove, int choice,int velo)//add 1 more class to change it speed
{
	Z2 = PrevZ;
	timer += dt;
	//speed 10 will be the class
	if (Y != 67 && checkhop == 0)
	{
		Y += 3 * dt;
		if (Y >= 67)
		{
			checkhop = 1;
		}
	}
	else if (Y != 64 && checkhop == 1)
	{
		Y -= 3 * dt;
		if (Y <= 64)
		{
			checkhop = 0;
		}
	}
	if (timer >10 && checkmove ==false)
	{
		timer = 0;
		checkmove = true;
		randNum=choicemaker(choice);
		if (randNum==PrevNum)
		{
			randNum = 0;
		}
	}
	if (randNum ==1)
	{
		PrevNum = 1;
		if (X<30)
		{
			X += float(velo * dt);
			if (X >= 30)
			{
				checkmove = false;
				X += float(0 * dt);
				randNum = 0;
			}
		}
	}
	else if (randNum == 2)
	{
		PrevNum = 2;
		if (X > -30)
		{
			X -= float(velo * dt);
			if (X <= -30)
			{
				checkmove = false;
				X -= float(0 * dt);
				randNum = 0;
			}
		}
	}
	else 
	{
		 Z1 = speed(velo,dt);
	}
	setpos(X, Y, Z1);


	return AI_Position;
}

Vector3 enemy2::setpos(float X, float Y, float Z)
{
	AI_Position = (Vector3(X, Y, Z));
	return AI_Position;
}

int enemy2::choicemaker(int choice)
{
	choice = rand() % 2 + 1;
	return choice;
}



