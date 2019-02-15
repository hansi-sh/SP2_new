#ifndef LIGHT_H
#define LIGHT_H

#include "Scene.h"
#include "Mesh.h"

struct Light
{
	Position position;
	Color color;
	float power;
	float kC, kL, kQ;

	Light()
	{
		color.Set(1.0f, 1.0f, 1.0f);
		power = 1.0f;
		kC = 1.0f;
		kL = 0.0f;
		kQ = 0.0f;
	}

	enum LIGHT_TYPE
	{
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL,
		LIGHT_SPOT,
	};

	LIGHT_TYPE type;
	Vector3 spotDirection;
	float cosCutoff;
	float cosInner;
	float exponent;



};

//Coscutoff = increase the area of light shown
//exponent = 

#endif // !LIGHT_H
