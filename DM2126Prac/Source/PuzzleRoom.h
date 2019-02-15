#ifndef PUZZLEROOM_H
#define PUZZLEROOM_H

#include "Scene.h"
#include "Camera2.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include <Windows.h>

class PuzzleRoom : public Scene
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
	PuzzleRoom();
	~PuzzleRoom();

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
	Vector3 currentCamPos;
	Vector3 currentCamTarget;
	bool getCurrentCam;

	void RenderSkybox();
	void RenderPokeCenter();

	bool b_BMO;
	bool b_viewStats;

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
	float elapsedtime = 0;
	float bouncetime = 0;
	float RotateDoor1 = 0;
	bool door1open;
	//<--Music-->
	void PlayMusic();
	bool b_musicSelected;
	bool b_inPM;
	bool b_checkinPM;
	bool b_inPC;

	enum GEOMETRY_TYPE
	{

		//GEO_AXES,
		GEO_LIGHTBALL,
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
		//GEO_BOOK1,
		//GEO_BOOK2,
		//GEO_BOOK3,
		//GEO_BOOK4,
		//GEO_BOOKBLACK,
		NUM_GEOMETRY,
	};

	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	static Camera2 camera;

	Light light[1];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
};
#endif