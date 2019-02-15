#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"

Camera2::Camera2()
{
}

Camera2::~Camera2()
{
}

void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	this->up = defaultUp = up;
}

void Camera2::Update(double dt)
{
	static const float CAMERA_SPEED = 50.f;
	Vector3 view = (target - position).Normalized();
	Vector3 horizontalView = view;
	horizontalView.y = 0.0f;
	horizontalView = (horizontalView).Normalized();
	Vector3 right = view.Cross(up);
	float cameraspeedchange = 0.1;
	if (Application::IsKeyPressed('A'))
	{
		position = position - right * cameraspeedchange;
		target = position + view * cameraspeedchange;
	}
	if (Application::IsKeyPressed('D'))
	{
		position = position + right * cameraspeedchange;
		target = position + view * cameraspeedchange;
	}
	if (Application::IsKeyPressed('Q'))
	{
		position = position + up * cameraspeedchange;
		target = position + view * cameraspeedchange;
	}
	if (Application::IsKeyPressed('E'))
	{
		position = position - up * cameraspeedchange;
		target = position + view * cameraspeedchange;
	}
	if (Application::IsKeyPressed('W'))
	{
		position += horizontalView * cameraspeedchange;
		target += horizontalView * cameraspeedchange;
	}
	if (Application::IsKeyPressed('S'))
	{
		position -= horizontalView * cameraspeedchange;
		target -= horizontalView * cameraspeedchange;
	}
	if (Application::IsKeyPressed(VK_LEFT))
	{
		Mtx44 rotation;
		float yaw = (float)(CAMERA_SPEED * dt);
		rotation.SetToRotation(yaw, up.x, up.y, up.z);

		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		Mtx44 rotation;
		float yaw = (float)(CAMERA_SPEED * dt);
		rotation.SetToRotation(-yaw, up.x, up.y, up.z);

		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed(VK_UP))
	{
		Mtx44 rotation;
		float pitch = (float)(CAMERA_SPEED * dt);
		rotation.SetToRotation(pitch, right.x, right.y, right.z);

		view = rotation * view;
		target = position + view;

	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		Mtx44 rotation;
		float pitch = (float)(CAMERA_SPEED * dt);
		rotation.SetToRotation(-pitch, right.x, right.y, right.z);

		view = rotation * view;
		target = position + view;
	}
	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}
}



void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}



/*
#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"
Camera2::Camera2()
{
}
Camera2::~Camera2()
{
}
void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}
void Camera2::Update(double dt)
{
	static const float CAMERA_SPEED = 50.f;
	if(Application::IsKeyPressed(VK_LEFT) || Application::IsKeyPressed('A'))
	{
		float yaw = (float)(-CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if(Application::IsKeyPressed(VK_RIGHT) || Application::IsKeyPressed('D'))
	{
		float yaw = (float)(CAMERA_SPEED * dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		position = rotation * position;
		up = rotation * up;
	}
	if(Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		float pitch = (float)(-CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if(Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		float pitch = (float)(CAMERA_SPEED * dt);
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		position = rotation * position;
	}
	if(Application::IsKeyPressed('Q'))
	{
		Vector3 direction = target - position;
		if(direction.Length() > 5)
		{
			Vector3 view = (target - position).Normalized();
			position += view * (float)(10.f * dt);
		}
	}
	if(Application::IsKeyPressed('E'))
	{
		Vector3 view = (target - position).Normalized();
		position -= view * (float)(10.f * dt);
	}
	if(Application::IsKeyPressed('R'))
	{
		Reset();
	}
}
void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}
*/