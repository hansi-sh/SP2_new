#ifndef RACING_H
#define RACING_H

#include "Scene.h"
#include "Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "ObjectBox.h"
#include "Enemy.h"
#include "enemy2.h"
#include <Windows.h>
#include "Physics.h"
#include "StopWatchTimer.h"
class RaceScene : public Scene
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
	RaceScene();
	~RaceScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float LSPEED;

	bool b_collide;
	bool b_AIcollide;

	int i_collider1;
	int i_collider2;

	float f_prevAngle;

	void RenderSkybox();

	//for enemy
	int i_randomMove[30];
	float f_RotateEnemyBody[30];
	float f_enemyX[30], f_enemyY[30], f_enemyZ[30];
	Enemy e[30];
	Vector3 V_enemyUpdatePos[30];
	bool b_StepENEMYAccelerator;
	bool b_StepENEMYBrakes;
	bool b_ENEMYSteer[30];
	bool b_randcheck[30];
	float f_ENEMYRotateAmt[30];

	//For Car
	float f_RotateBody;
	float f_TranslateBodyX;
	float f_TranslateBodyY;
	float f_TranslateBodyZ;
	Physics P_PlayerCar;
	Vector3 V_UpdatedPlayerPos;
	bool b_StepAccelerator;
	bool b_StepBrakes;
	bool b_Steer;
	float f_RotateAmt;
	float f_UpdatedAngle;
	int i_CollidedWith;
	int i_CollidedWith2;

	//AI
	bool b_dead;
	bool b_collideAI;
	enemy2 e_AIwalker[30];
	bool b_checkmove[30];
	float f_AIWalkX[30];
	float f_AIWalkY[30];
	float f_AIWalkZ[30];
	Vector3 V_AIpos[30];
	int i_movechoice[30];

	bool b_movement;
	bool b_loading;
	// stuff for sound:
	bool b_Warning; // print the out of bound
	bool b_AlertSound; // for warning sound
	double d_Delay;
	int i_CountDown;

	float f_HeightAIP;	//AI + players car height

	//pausemenu
	bool b_pause;
	double d_BounceTime;

	enum GEOMETRY_TYPE
	{
		GEO_LIGHTBALL,
		GEO_AICUBE,
		GEO_Pedestrains1,
		GEO_Pedestrains2,
		GEO_Pedestrains3,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_HOSPITAL,
		GEO_RACETRACK,
		GEO_AMBULANCE,
		GEO_SPEEDMETER,
		GEO_TIME,
		GEO_PAUSE,
		GEO_PAUSESELECT,

		GEO_CAR1,
		GEO_CAR2,
		GEO_CAR3,

		//GEO_BOX1,
		//GEO_BOX2,
		
		GEO_WARNING, // if car onto pavement
		GEO_START,

		GEO_TEXT,
		GEO_LOADING,
		NUM_GEOMETRY,
	};

	enum OBJECT_TYPE
	{
		OBJ_PLAYER,
		OBJ_BOX1,
		OBJ_BOX2,
		OBJ_walker1,   //Golden mushroom
		OBJ_walker2,   //Golden mushroom
		OBJ_walker3,   //Golden mushroom
		OBJ_walker4,   //Golden mushroom
		OBJ_walker5,   //Golden mushroom
		OBJ_walker6,   //Golden mushroom
		OBJ_walker7,   //Golden mushroom
		OBJ_walker8,   //Golden mushroom
		OBJ_walker9,   //Golden mushroom
		OBJ_walker10,  //Golden mushroom
		k,  //Normal mushroom
		OBJ_walker12,  //Normal mushroom
		OBJ_walker13,  //Normal mushroom
		OBJ_walker14,  //Normal mushroom
		OBJ_walker15,  //Normal mushroom
		OBJ_walker16,  //Normal mushroom
		OBJ_walker17,  //Normal mushroom
		OBJ_walker18,  //Normal mushroom
		OBJ_walker19,  //Normal mushroom
		OBJ_walker20,  //Normal mushroom
		OBJ_walker21,  //Normal mushroom
		OBJ_walker22,  //Normal mushroom
		OBJ_walker23,  //Normal mushroom
		OBJ_walker24,  //Normal mushroom
		OBJ_walker25,  //Normal mushroom
		OBJ_walker26,  //Normal mushroom
		OBJ_walker27,  //Normal mushroom
		OBJ_walker28,  //Normal mushroom
		OBJ_walker29,  //Normal mushroom
		OBJ_walker30,  //Normal mushroom
		OBJ_ENEMY1,
		OBJ_ENEMY2,
		OBJ_ENEMY3,
		OBJ_ENEMY4,
		OBJ_ENEMY5,
		OBJ_ENEMY6,
		OBJ_ENEMY7,
		OBJ_ENEMY8,
		OBJ_ENEMY9,
		OBJ_ENEMY10,
		OBJ_ENEMY11,
		OBJ_ENEMY12,
		OBJ_ENEMY13,
		OBJ_ENEMY14,
		OBJ_ENEMY15,

		NUM_OBJ
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	ObjectBox* Obj[NUM_OBJ];

	Camera3 camera;
	float f_TPCRotateBy;
	int i_Selector;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y);

	StopWatchTimer RaceTimer;

	bool b_showIntro;
	double d_score;
	bool timerunout;
};

#endif