#ifndef ASSIGNMENTSCENE_H
#define ASSIGNMENTSCENE_H

#include "Scene.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include <Windows.h>
#include "ObjectBox.h"

class AssignmentScene : public Scene
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
	AssignmentScene();
	~AssignmentScene();

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static float lastX, lastY;

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void RenderButton(int, int);
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	float LSPEED;
	float fps;
	bool getCurrentCam;
	float speed;

	void RenderSkybox();
	void RenderPokeCenter();
	
	bool b_BMO;
	bool b_viewStats;

	bool collide;
	Vector3 currentCamPos;
	Vector3 currentCamTarget;

	float rotationangle;
	float updatedangle;
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
	void PlayMusic();
	bool b_musicSelected;
	bool b_inPM;
	bool b_checkinPM;
	bool b_inPC;

	enum GEOMETRY_TYPE
	{
		GEO_LIGHTBALL,
		GEO_AXES,
		GEO_BODY,
		GEO_FACE,
		GEO_FACEOFF,
		GEO_ARMS,
		GEO_HAND,
		GEO_LEG,
		GEO_FEET,
		GEO_EYESCIRCLE,
		GEO_EYESCYLINDER,
		GEO_MOUTHSEMICIRCLE,
		GEO_MOUTHCYLINDER,
		GEO_MOUTHQUAD,
		GEO_USB,
		GEO_YELLOWBUTTONCUBOID,
		GEO_BLUEBUTTONCIRCLE,
		GEO_BLUEBUTTONCYLINDER,
		GEO_GREENBUTTONCIRCLE,
		GEO_GREENBUTTONCYLINDER,
		GEO_REDBUTTONCIRCLE,
		GEO_REDBUTTONCYLINDER,
		GEO_TRIPRISM,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_POKECWALLWW,
		GEO_POKECEILING,
		GEO_POKECFLOOR,
		GEO_BOX0,
		GEO_BOX,
		GEO_BOX2,
		GEO_TEXT,
		NUM_GEOMETRY,
	};

	enum OBJECT_TYPE
	{
		OBJ_BOX2,	//player 0
		OBJ_BOX0,	//1
		OBJ_BOX,	//2

		NUM_OBJ
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	ObjectBox* Obj[NUM_OBJ];

	static Camera2 camera;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

};

#endif