#pragma once
#include <Vector3.h>
#include <Mtx44.h>
#include <cmath>

class ObjectBox
{
private:
	Vector3 Pos, xAxis, yAxis, zAxis;
	float Width, Height, Depth;

protected:
public:
	void setOBB(Vector3 Position);
	void setRotatingAxis(float degree, float xAxis, float yAxis, float zAxis);
	static bool checkCollision(const ObjectBox A, const ObjectBox B);
	ObjectBox();
	ObjectBox(Vector3 Position, float width, float height, float depth);	//Takes in Position, width, height and depth of object [How big it is in the world and where]
	~ObjectBox();
};
