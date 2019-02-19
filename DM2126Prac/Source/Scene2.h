#ifndef SCENE2_H
#define SCENE2_H

#include "Scene.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include <Windows.h>
#include "ObjectBox.h"

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

	void RenderSkybox();

	bool b_BMO;
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

	//For Running Animation
	bool checkmodelStack;
	float RotateBody;
	bool running;
	bool bodyMovement;
	float LeftLegX;
	float RightLegX;
	float ArmRotation;
	float TranslateBodyX;
	float TranslateBodyY;
	float TranslateBodyZ;

	//<--Music-->
	// void PlayMusic();
	bool b_musicSelected;
	bool b_inPM;
	bool b_checkinPM;
	bool b_inPC;

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
		//GEO_TEST, // Cabinet2

		GEO_TEXT,
		NUM_GEOMETRY,
	};

	enum OBJECT_TYPE
	{
		OBJ_PLAYER,	//player 0

		OBJ_METALSHELVE, // -> done
		OBJ_STRETCHER, // - > done
		OBJ_CABINET, // - > done
		OBJ_CHAIR, // - > done
		OBJ_DEFIBRILLATOR, // -> done
		OBJ_FIRSTAIDKIT, // -> done
		OBJ_CABINET2, // -> done
		OBJ_TOPSHELVE1, // - > done
		OBJ_TOPSHELVE2, // - > done
		OBJ_TOPSHELVE3, // - > done

		//OBJ_TEST, // Cabinet2

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

	bool setTrueFalse = false;
	// for Cabinet 2
	// when on light the grooves show
	// off light grooves doesnt show
};

#endif