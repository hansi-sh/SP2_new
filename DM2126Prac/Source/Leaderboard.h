#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "Scene.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "item.h"
#include <Windows.h>
#include "ObjectBox.h"
#include <fstream>
using namespace std;

class Leaderboard : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_NUMLIGHTS,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	Leaderboard();
	~Leaderboard();

	static float lastX, lastY;

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float f_LSPEED;
	float f_fps;
	bool b_getCurrentCam;
	float f_speed;
	double d_delay;

	bool b_viewStats;

	// For Box collision
	bool b_collide;
	Vector3 currentCamPos;
	Vector3 currentCamTarget;

	float f_rotationangle; // not needed but jus leave here
	float f_updatedangle; // not needed but jus leave here
	float f_prevBodyX;
	float f_prevBodyZ;
	float f_prevAngle;

	enum GEOMETRY_TYPE
	{
		GEO_TEXT,
		GEO_BACKGROUND,
		GEO_BORDER,
		NUM_GEOMETRY,
	};

	enum OBJECT_TYPE
	{
		OBJ_PLAYER,	//player 0

		NUM_OBJ
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	ObjectBox* Obj[NUM_OBJ]; // for box collision

	static Camera2 camera;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

	// <<---Leaderboard stuff--->>
	float i_RNG;
	float i_leaderboard[100];
	int i_counter = 0;
	float i_first = 0;
	float i_second = 0;
	float i_third = 0;
};

#endif