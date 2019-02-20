#include "Physics.h"


Physics::Physics()
{
	f_CarSpeed = 0;
	f_Acceleration = 20.0f; //constant acceleration
	V_Position = Vector3(0, 0, 0);
	V_Direction = Vector3(0.0f, 0.0f, 1.0f);
}


Physics::~Physics()
{
}

void Physics::v_SetPos(Vector3 pos)
{
	this->V_Position = pos; //init pos
}

void Physics::v_UpdateCarSpeed(bool stepAccelerator, bool stepBrakes, bool steerWheel, double dt) //update speed
{
	if (stepAccelerator)
		f_CarSpeed += float(f_Acceleration * dt);
		//Increase f_CarSpeed in terms of acceleration and time (v=u+at)
	else if(stepBrakes)
		f_CarSpeed -= float(f_Acceleration * dt);
		//Decrease f_CarSpeed in terms of acceleration and time (v=u+at)
	
	if (steerWheel)//if rotate
	{
		//Decrease f_CarSpeed when rotating
		if(f_CarSpeed > 0.0)
			f_CarSpeed = f_CarSpeed - 0.05f; //setspeed=getspeed-0.05
		else
			f_CarSpeed = f_CarSpeed + 0.05f;
	}

	//Deceleration always trys to slow car down
	f_CarSpeed -= f_CarSpeed * 0.2 * (dt);

}

void Physics::v_UpdateCarDirection(float Degree, float previousDegree) //caculate rotation
{
	Mtx44 rotateBy;
	rotateBy.SetToRotation((Degree - previousDegree), 0, 1, 0);

	V_Direction = (rotateBy * V_Direction).Normalize();
}

float Physics::f_GetSpeed()//get speed in AS scene
{
	return f_CarSpeed;
}

void Physics::v_SetSpeed(float newSpeed) // set speed in AS scene
{
	f_CarSpeed = newSpeed;
}

Vector3 Physics::V_UpdateCarPos(double dt) //update current position.
{
	V_Position += f_CarSpeed * V_Direction * dt;
	return V_Position;
}

//Gravity will affect the y axis position of player 