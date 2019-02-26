#ifndef TUTORIALSCENE2_H
#define TUTORIALSCENE2_H

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "item.h"
#include <Windows.h>
#include <fstream>
using namespace std;

class TutorialScene2 : public Scene
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
	TutorialScene2();
	~TutorialScene2();

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

	void RenderSkybox();

	enum GEOMETRY_TYPE
	{
		GEO_LIGHTBALL,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_STRETCHER,
		GEO_DEFIBRILLATOR, 
		GEO_FIRSTAIDKIT,
		GEO_CABINET,
		GEO_CABINET2,
		GEO_CHAIR,
		GEO_TOPSHELVE1,
		GEO_TOPSHELVE2,
		GEO_TOPSHELVE3,
		GEO_METALSHELVE, 

		GEO_PLAYER,
		GEO_TEXT,

		GEO_HELP, // the top right panel
		GEO_FRAME, // the frame to contain text
		GEO_START, // ambulance
		GEO_START2,
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

		NUM_GEOMETRY,
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	Camera3 camera;
	float f_TPCRotateBy;
	bool b_Switch;
	double d_BounceTime;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

	bool b_SetTrueFalse;
};

#endif