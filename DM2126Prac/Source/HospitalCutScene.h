#ifndef HospitalCutScene_H
#define HospitalCutScene_H
#pragma once
#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "ObjectBox.h"
#include <Windows.h>
#include "Physics.h"
class HospitalCutScene : public Scene
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
	HospitalCutScene();
	~HospitalCutScene();
	static float f_lastX, f_lastY;

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	//player
	bool b_parking;
	bool b_out;
	float f_playerX;
	float f_playerY;
	float f_playerZ;
	float f_playerX2;
	float f_playerY2;
	float f_playerZ2;

	//bed
	float f_bedX;
	float f_bedY;
	float f_bedZ;

	//paient
	float f_paientX;
	float f_paientY;
	float f_paientZ;

	//delay
	double d_delayed;

	void RenderButton(int, int);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float f_LSPEED;
	float f_fps;
	Vector3 currentCamPos;
	Vector3 currentCamTarget;
	bool b_getCurrentCam;

	void RenderSkybox();

	//For Car
	float f_RotateBody;
	float f_TranslateBodyX;
	float f_TranslateBodyY;
	float f_TranslateBodyZ;

	float f_HeightAIP;	//AI + players car height

	enum GEOMETRY_TYPE
	{
		GEO_LIGHTBALL,
		GEO_NURSE,
		GEO_HAIR, // patient
		GEO_FACE,
		GEO_BODY,
		GEO_RARM,
		GEO_LARM,

		GEO_RHAND,
		GEO_LHAND,
		GEO_RLEG,
		GEO_LLEG,
		GEO_CROTCH,
		GEO_STRETCHER,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_HOSPITAL,
		GEO_RACETRACK,
		GEO_AMBULANCE,
		GEO_TEXT,
		NUM_GEOMETRY,
	};
	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];
	Camera3 camera;
	float f_TPCRotateBy;
	bool b_Switch;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

};
#endif
