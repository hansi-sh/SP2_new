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
		//U_LIGHT1_POSITION,
		//U_LIGHT1_COLOR,
		//U_LIGHT1_POWER,
		//U_LIGHT1_KC,
		//U_LIGHT1_KL,
		//U_LIGHT1_KQ,
		//U_LIGHT1NABLED,
		//U_LIGHT1_TYPE,
		//U_LIGHT1_SPOTDIRECTION,
		//U_LIGHT1_COSCUTOFF,
		//U_LIGHT1_COSINNER,
		//U_LIGHT1_EXPONENT,
		//U_LIGHT2_POSITION,
		//U_LIGHT2_COLOR,
		//U_LIGHT2_POWER,
		//U_LIGHT2_KC,
		//U_LIGHT2_KL,
		//U_LIGHT2_KQ,
		//U_LIGHT2NABLED,
		//U_LIGHT2_TYPE,
		//U_LIGHT2_SPOTDIRECTION,
		//U_LIGHT2_COSCUTOFF,
		//U_LIGHT2_COSINNER,
		//U_LIGHT2_EXPONENT,
		//U_LIGHT3_POSITION,
		//U_LIGHT3_COLOR,
		//U_LIGHT3_POWER,
		//U_LIGHT3_KC,
		//U_LIGHT3_KL,
		//U_LIGHT3_KQ,
		//U_LIGHT3NABLED,
		//U_LIGHT3_TYPE,
		//U_LIGHT3_SPOTDIRECTION,
		//U_LIGHT3_COSCUTOFF,
		//U_LIGHT3_COSINNER,
		//U_LIGHT3_EXPONENT,
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
	float LSPEED;
	float fps;
	bool getCurrentCam;
	float speed;
	double delay;

	void RenderSkybox();

	bool b_viewStats;

	// For Box collision
	bool collide;
	Vector3 currentCamPos;
	Vector3 currentCamTarget;

	float rotationangle; // not needed but jus leave here
	float updatedangle; // not needed but jus leave here
	float prevBodyX;
	float prevBodyZ;
	float prevAngle;

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

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	// Stuff Added
	bool setTrueFalse = false;
	int instruction;
	bool collectDefi = false;
	bool collectKit = false;
	bool notification1 = false;
	bool notification2 = false;
	bool useDefi = false; 
	bool useKit = false;
	bool nextStage = false;
	double score = 1.0;
	bool showIntro = true;
	bool useSound = true;

	void RenderMission();
	void EndMission();
	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

	void uploadItem(int);
	void printNext();
	void printPrev();
	void rendertag();
	Item *first, *last, *forward, *current, *backward;
	//Timer
	StopWatchTimer* AmbulanceTimer;
	bool timerunout = false;

};

#endif