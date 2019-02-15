#include "ObjectBox.h"



ObjectBox::ObjectBox()
{
}

ObjectBox::ObjectBox(Vector3 Position, float width, float height, float depth)
{
	Pos = Position;
	Width = width / 2;
	Height = height / 2;
	Depth = depth / 2;
	xAxis = Vector3(1.0f, 0.0f, 0.0f);
	yAxis = Vector3(0.0f, 1.0f, 0.0f);
	zAxis = Vector3(0.0f, 0.0f, 1.0f);
}

ObjectBox::~ObjectBox()
{
}

void ObjectBox::setOBB(Vector3 Position) //sets OBB when object moves
{
	Pos = Position;
}

bool ObjectBox::checkCollision(const ObjectBox A, const ObjectBox B)
{
	Vector3 T = B.Pos - A.Pos;

	return !(
		fabs(T.Dot(A.xAxis)) > (A.Width + fabs(B.Width * (A.xAxis.Dot(B.xAxis))) + fabs(B.Height * (A.xAxis.Dot(B.yAxis))) + fabs(B.Depth * (A.xAxis.Dot(B.zAxis))))													||		//Case 1
		fabs(T.Dot(A.yAxis)) > (A.Height + fabs(B.Width * (A.yAxis.Dot(B.xAxis))) + fabs(B.Height * (A.yAxis.Dot(B.yAxis))) + fabs(B.Depth * (A.yAxis.Dot(B.zAxis))))													||		//Case 2
		fabs(T.Dot(A.zAxis)) > (A.Depth + fabs(B.Width * (A.zAxis.Dot(B.xAxis))) + fabs(B.Height * (A.zAxis.Dot(B.yAxis))) + fabs(B.Depth * (A.zAxis.Dot(B.zAxis)))) 													||		//Case 3
		fabs(T.Dot(B.xAxis)) > (B.Width + fabs(A.Width * (A.xAxis.Dot(B.xAxis))) + fabs(A.Height * (A.yAxis.Dot(B.xAxis))) + fabs(A.Depth * (A.zAxis.Dot(B.xAxis)))) 													||		//Case 4
		fabs(T.Dot(B.yAxis)) > (B.Height + fabs(A.Width * (A.xAxis.Dot(B.yAxis))) + fabs(A.Height * (A.yAxis.Dot(B.yAxis))) + fabs(A.Depth * (A.zAxis.Dot(B.yAxis))))													||		//Case 5
		fabs(T.Dot(B.zAxis)) > (B.Depth + fabs(A.Width * (A.xAxis.Dot(B.zAxis))) + fabs(A.Height * (A.yAxis.Dot(B.zAxis))) + fabs(A.Depth * (A.zAxis.Dot(B.zAxis))))													||		//Case 6
		fabs(T.Dot(A.xAxis.Cross(B.xAxis))) > ( fabs(A.Height * (A.zAxis.Dot(B.xAxis))) + fabs(A.Depth * (A.yAxis.Dot(B.xAxis))) + fabs(B.Height * (A.xAxis.Dot(B.zAxis))) +  fabs(B.Depth * (A.xAxis.Dot(B.yAxis))) )	||		//Case 7
		fabs(T.Dot(A.xAxis.Cross(B.yAxis))) > ( fabs(A.Height * (A.zAxis.Dot(B.yAxis))) + fabs(A.Depth * (A.yAxis.Dot(B.yAxis))) + fabs(B.Width * (A.xAxis.Dot(B.zAxis))) + fabs(B.Depth * (A.xAxis.Dot(B.xAxis))) )	||		//Case 8
		fabs(T.Dot(A.xAxis.Cross(B.zAxis))) > ( fabs(A.Height * (A.zAxis.Dot(B.zAxis))) + fabs(A.Depth * (A.yAxis.Dot(B.zAxis))) + fabs(B.Width * (A.xAxis.Dot(B.yAxis))) + fabs(B.Height * (A.xAxis.Dot(B.xAxis))))	||		//Case 9
		fabs(T.Dot(A.yAxis.Cross(B.xAxis))) > ( fabs(A.Width * (A.zAxis.Dot(B.xAxis))) + fabs(A.Depth * (A.xAxis.Dot(B.xAxis))) + fabs(B.Height * (A.yAxis.Dot(B.zAxis))) + fabs(B.Depth * (A.yAxis.Dot(B.yAxis))))		||		//Case 10
		fabs(T.Dot(A.yAxis.Cross(B.yAxis))) > ( fabs(A.Width * (A.zAxis.Dot(B.yAxis))) + fabs(A.Depth * (A.xAxis.Dot(B.yAxis))) + fabs(B.Width * (A.yAxis.Dot(B.zAxis))) + fabs(B.Depth * (A.yAxis.Dot(B.xAxis))))		||		//Case 11
		fabs(T.Dot(A.yAxis.Cross(B.zAxis))) > ( fabs(A.Width * (A.zAxis.Dot(B.zAxis))) + fabs(A.Depth * (A.xAxis.Dot(B.zAxis))) + fabs(B.Width * (A.yAxis.Dot(B.yAxis))) + fabs(B.Height * (A.yAxis.Dot(B.xAxis))))		||		//Case 12
		fabs(T.Dot(A.zAxis.Cross(B.xAxis))) > ( fabs(A.Width * (A.yAxis.Dot(B.xAxis))) + fabs(A.Height * (A.xAxis.Dot(B.xAxis))) + fabs(B.Height * (A.zAxis.Dot(B.zAxis))) + fabs(B.Depth * (A.zAxis.Dot(B.yAxis))))	||		//Case 13
		fabs(T.Dot(A.zAxis.Cross(B.yAxis))) > ( fabs(A.Width * (A.yAxis.Dot(B.yAxis))) + fabs(A.Height * (A.xAxis.Dot(B.yAxis))) + fabs(B.Width * (A.zAxis.Dot(B.zAxis))) + fabs(B.Depth * (A.zAxis.Dot(B.xAxis))))		||		//Case 14
		fabs(T.Dot(A.zAxis.Cross(B.zAxis))) > ( fabs(A.Width * (A.yAxis.Dot(B.zAxis))) + fabs(A.Height * (A.xAxis.Dot(B.zAxis))) + fabs(B.Width * (A.zAxis.Dot(B.yAxis))) + fabs(B.Height * (A.zAxis.Dot(B.xAxis)))) 			//Case 15
		);
}

void ObjectBox::setRotatingAxis(float degree, float x, float y, float z)
{
	Mtx44 rotateBy;
	rotateBy.SetToRotation(degree, x, y, z); //to find the rotational matrix

	//to multiply the local axis (X/Y/Z) with the rotational matrix to know how much to rotate by and normalize it to make it a unit vector
	xAxis = (rotateBy * xAxis).Normalize(); 
	yAxis = (rotateBy * yAxis).Normalize();
	zAxis = (rotateBy * zAxis).Normalize();

}