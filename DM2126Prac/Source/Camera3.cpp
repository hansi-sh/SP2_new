#include "Camera3.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	this->up = defaultUp = up;
	this->offSet = Vector3(0, 40, -70);	//How far it is away from object
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
}

void Camera3::Update(float degree, float x, float y, float z)
{
	this->target = Vector3(x, y, z);	//always looking at players position;
	rotateArndObj.SetToRotation(degree, 0, 1, 0); //rotate in the y axis in the degree
	this->offSet = rotateArndObj * offSet;
	this->position = target + offSet;
}



void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}
