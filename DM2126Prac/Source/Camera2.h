#ifndef CAMERA_2_H
#define CAMERA_2_H

#include "Camera.h"
#include "Application.h"

class Camera2 : public Camera
{
public:
	Application app;
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera2();
	~Camera2();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt, bool move);
	virtual void Reset();
};

#endif