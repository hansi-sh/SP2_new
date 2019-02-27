#ifndef SCENE2_H
#define SCENE2_H

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
#include "StopWatchTimer.h"

#include <fstream>
using namespace std;

class Scene2 : public Scene
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
	Scene2();
	~Scene2();

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
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

	void RenderSkybox();
	bool b_viewStats;

	//pausemenu
	bool b_pause;
	double d_BounceTime;

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
		GEO_LIGHTBALL,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_STRETCHER,
		GEO_DEFIBRILLATOR, // jus a cube
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
		GEO_TIME,
		GEO_PAUSE,
		GEO_PAUSESELECT,
		NUM_GEOMETRY,
	};

	enum OBJECT_TYPE
	{
		OBJ_PLAYER,	//player 0

		OBJ_METALSHELVE, // -> done
		OBJ_STRETCHER, // - > done
		OBJ_CABINET, // - > done
		OBJ_CHAIR, // - > done
		OBJ_CABINET2, // -> done
		OBJ_TOPSHELVE1, // - > done
		OBJ_TOPSHELVE2, // - > done
		OBJ_TOPSHELVE3, // - > done

		OBJ_FRONT, // - > done
		OBJ_BACK, 
		OBJ_TOP, 
		OBJ_BOTTOM,
		OBJ_LEFT,
		OBJ_RIGHT,

		NUM_OBJ
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	ObjectBox* Obj[NUM_OBJ]; // for box collision

	static Camera2 camera;
	int i_Selector;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	// Stuff Added
	bool b_setTrueFalse;
	int i_instruction;
	bool b_collectDefi;
	bool b_collectKit;
	bool b_notification1;
	bool b_notification2;
	bool b_useDefi; 
	bool b_useKit;
	bool b_nextStage;
	double d_score;
	bool b_showIntro;
	bool b_useSound;

	void RenderMission();
	void EndMission();
	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

	void uploadItem(int);
	void printNext();
	void printPrev();
	void rendertag();

	bool b_itemcollect;
	int i_itemcount;
	int i_totalItem;

	Item *first, *last, *forward, *current, *backward;
	//Timer
	StopWatchTimer* AmbulanceTimer;
	bool b_timerunout;
	bool b_defiCollected;
	bool b_firstAidKitCollected;
};

#endif