#include "Vector3.h"
#include "Mtx44.h"

class Physics
{
private:
	float f_CarSpeed;
	float f_Acceleration;
	Vector3 V_Position;																	//Which will be constantly updated base on velocity for car
	Vector3 V_Direction;																//Which will be constantly updated base on angle of rotation (Speed + direction)
public:
	Physics();
	~Physics();
	void v_SetPos(Vector3 pos);															//To initialize position of player [Car/camera pos] if there is gravity
	void v_UpdateCarSpeed(bool stepAccelerator, bool stepBrakes,bool steerWheel, double dt);			//To calculate pos after velocity is added to the current positon for car
	void v_UpdateCarDirection(float Degree, float previousDegree);						//To reposition the car
	float f_GetSpeed();																	//To get current speed value
	void v_SetSpeed(float newSpeed);													//To update its speed, if affected		
	Vector3 V_UpdateCarPos(double dt);															//Get position function for players car
};

//Create a bool in SP Scene3 to increase speed if true, decrease speed to 0 if false;