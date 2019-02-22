#include "enemy2.h"
#include <stdlib.h> 


enemy2::enemy2()
{
	AI_Position = Vector3(0, 0, 0);
	X1 = 0;//currentX
	X2 = 0;//prevX
	Y = 0;
	Z = 0;
}


enemy2::~enemy2()
{
}

Vector3 enemy2::walking(float prevX)
{
	/*if(x2-x1 >15)
	yincrease value
	x2=x1*/
	return AI_Position;
}



