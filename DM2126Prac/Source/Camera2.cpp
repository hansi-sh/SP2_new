#include "Camera2.h"
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
	float cameraspeedchange = 1.0f;
	
	//if (app.GetSceneNumber() >= 1 && app.GetSceneNumber() <= 2)
	{
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
	}
}



void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}