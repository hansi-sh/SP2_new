#include "enemy2.h"
#include <stdlib.h> 


enemy2::enemy2()
{
	D_timer = 0;
	F_check = 0;
	F_checkhop = 0;
	V_AI_Position = Vector3(0, 0, 0);
	F_Z1 = 0;//currentz
	F_Z2 = 0;//prevz
	F_walkingspeed = 0;
}


enemy2::~enemy2()
{
}

float enemy2::speed(int velo,float dt)
{
	F_walkingspeed = F_Z2;
	if (V_AI_Position.z!=1450&& F_check ==0)
	{
		F_walkingspeed += float(velo * dt);
		if (V_AI_Position.z >= 1450)
		{
			F_check = 1;
		}
		return F_walkingspeed;
	}
	 if(V_AI_Position.z != -1350&& F_check ==1)
	{
		 F_walkingspeed -= float(velo * dt);
		if (V_AI_Position.z <= -1350)
		{
			F_check = 0;
		}
		return F_walkingspeed;
	}
	
}

Vector3 enemy2::walking(float X, float Y, float PrevZ, double dt, bool checkmove, int choice,int velo)//add 1 more class to change it speed
{
	F_Z2 = PrevZ;
	D_timer += dt;
	//speed 10 will be the class
	if (Y != 67 && F_checkhop == 0)
	{
		Y += 3 * dt;
		if (Y >= 67)
		{
			F_checkhop = 1;
		}
	}
	else if (Y != 64 && F_checkhop == 1)
	{
		Y -= 3 * dt;
		if (Y <= 64)
		{
			F_checkhop = 0;
		}
	}
	if (D_timer >10 && checkmove ==false)
	{
		D_timer = 0;
		checkmove = true;
		I_randNum =choicemaker(choice);
		if (I_randNum == I_PrevNum)
		{
			I_randNum = 0;
		}
	}
	if (I_randNum ==1)
	{
		I_PrevNum = 1;
		if (X<30)
		{
			X += float(velo * dt);
			if (X >= 30)
			{
				checkmove = false;
				X += float(0 * dt);
				I_randNum = 0;
			}
		}
	}
	else if (I_randNum == 2)
	{
		I_PrevNum = 2;
		if (X > -30)
		{
			X -= float(velo * dt);
			if (X <= -30)
			{
				checkmove = false;
				X -= float(0 * dt);
				I_randNum = 0;
			}
		}
	}
	else 
	{
		F_Z1 = speed(velo,dt);
	}
	setpos(X, Y, F_Z1);


	return V_AI_Position;
}

Vector3 enemy2::setpos(float X, float Y, float Z)
{
	V_AI_Position = (Vector3(X, Y, Z));
	return V_AI_Position;
}

int enemy2::choicemaker(int choice)
{
	choice = rand() % 2 + 1;
	return choice;
}



