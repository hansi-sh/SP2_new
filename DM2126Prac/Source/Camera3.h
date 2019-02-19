#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"

class Camera3 : public Camera
{
public:

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera3();
	~Camera3();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(float degree, float x, float y, float z); //third person camera should take it X and Z position of player to update it and degree to rotate camera by
	virtual void Reset();
};

#endif