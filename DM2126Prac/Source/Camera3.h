#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"

class Camera3 : public Camera
{
private:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;
	Vector3 offSet;
	Mtx44 rotateArndObj;

public:

	Camera3();
	~Camera3();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(float degree, float x, float y, float z);
	virtual void Reset();
};

#endif