#ifndef TUTORIALPUZZLEROOM_H
#define TUTORIALPUZZLEROOM_H
#include <iostream>
#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include <Windows.h>
#include "ObjectBox.h"
#include "item.h"
#include "StopWatchTimer.h"

class TutorialPuzzleRoom : public Scene
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
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1NABLED,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_NUMLIGHTS,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
		
	};
public:
	TutorialPuzzleRoom();
	~TutorialPuzzleRoom();

	static float f_lastX, f_lastY;

	virtual void Init();
	void Update(double dt);
	// virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	//item
	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);
	
private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float f_LSPEED;
	float f_fps;

	void RenderSkybox();
	void CreepyHouse();
	
	//Light
	bool b_switchoneint;
	bool b_lightoneon;
	bool b_switchtwoint;
	bool b_lighttwoon;
	
	
	//For Running Animation
	bool b_checkmodelStack;
	//TV Drawer
	float f_drawertranslation = 0;
	bool b_drawerint;
	bool b_draweropen = false;
	//secret door
	float f_secretdoortranslation = 0;
	bool b_secretdooropen;
	bool b_secretdoorint ;
	bool b_lockeddoortext;
	//Pillow
	float f_pillowtranslation = 0;
	bool b_pillowint;
	bool b_pillowmoved = false;
	//door1
	float f_RotateDoor1=0;
	bool b_door1open;
	bool b_doorint ;
	//key1
	bool key1int;
	bool havekey1 = false;
	//key2
	bool b_key2int;
	bool b_havekey2 = false;
	//key3
	bool havekey3;
	//Patient
	bool b_patientint;
	bool b_patienthint;
	bool b_havepatient = false;
	//safe
	float f_rotatesafedoor;
	bool b_safeint = false;
	bool b_safeopen = false;
	bool b_safecracking = false;
	bool b_codecracked = false;
	bool b_wrongcode;
	//Promtp on screen
	bool b_interaction;
	bool b_interactioncomplete = false;
	bool b_paintingint;
	//codepress
	bool b_eight = false;
	bool   b_six = false;
	bool   b_one = false;
	bool   b_two = false;
	bool b_other = false;
	//final door
	bool b_finaldoorint;
	bool b_doorunlocked = false;

	bool b_showIntro = true; // Raphael Added
	double d_score = 1.0; // Raphael Added

	enum GEOMETRY_TYPE
	{
		
		//GEO_AXES,
		GEO_LIGHTBALL1,
		GEO_LIGHTBALL2,
		GEO_HOUSE,
		GEO_ROOM1WALL,
		GEO_DOORTOROOM1,
		GEO_SOFA,
		GEO_TEXT,
		GEO_PAINTINGKEY,
		GEO_PAINTINGFRUIT,
		GEO_PAINTINGCASTLE,
		GEO_PAINTINGFOREST,
		GEO_DOORENTRANCE,
		GEO_TV,
		GEO_TVTABLE,
		GEO_TVTABLEDRAWER,
		GEO_BOOKSHELF,
		GEO_BOOK1,
		GEO_BOOK2,
		GEO_BOOK3,
		GEO_BOOK4,
		GEO_BOOKBLACK,
		GEO_MATTRESS,
		GEO_BEDFRAME,
		GEO_PILLOW,
		GEO_SECRETWALL,
		GEO_SAFE,
		GEO_SAFEDOOR,
		GEO_KEY1,
		GEO_TABLEPAINTING,
		GEO_SWITCHONE,
		GEO_SWITCHTWO,
		GEO_KEY2,
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
		GEO_START, // Raphael Added
		GEO_START2,
		NUM_GEOMETRY,
	};
	
	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	Camera3 camera;
	float f_TPCRotateBy;
	bool b_Switch;
	double d_BounceTime;
	bool b_SwitchRotation;
	float f_TPCRotateTotal;

	Light light[2];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	//TimerClass
	StopWatchTimer* PuzzleTimer;
};
#endif
