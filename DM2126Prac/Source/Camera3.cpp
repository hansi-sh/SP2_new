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
}

void Camera3::Update(float degree, float x, float y, float z)
{
	Mtx44 rotateBy;
	rotateBy.SetToRotation(degree, 0, 1, 0);

	position = rotateBy * position;
}



void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}