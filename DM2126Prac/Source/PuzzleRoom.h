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
#include "ObjectBox.h"

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
	bool getCurrentCam;

	void RenderSkybox();
	void CreepyHouse();

	bool b_BMO;
	bool b_viewStats;
	//For collision
	bool collide;
	Vector3 currentCamPos;
	Vector3 currentCamTarget;
	
	//Light
	bool switchoneint;
	bool lightoneon;
	bool switchtwoint;
	bool lighttwoon;
	//time
	float elapsedtime = 0;
	float bouncetime = 0;
	//For Running Animation
	bool checkmodelStack;
	//TV Drawer
	float drawertranslation = 0;
	bool drawerint;
	bool draweropen = false;
	//secret door
	float secretdoortranslation = 0;
	bool secretdooropen;
	bool secretdoorint ;
	bool lockeddoortext;
	//Pillow
	float pillowtranslation = 0;
	bool pillowint;
	bool pillowmoved = false;
	//door1
	float RotateDoor1=0;
	bool door1open;
	bool doorint ;
	//key1
	bool key1int;
	bool havekey1 = false;
	//key2
	bool key2int;
	bool havekey2 = false;
	//safe
	float rotatesafedoor;
	bool safeint;
	bool safeopen;
	bool havekey3;
	//Promtp on screen
	bool interaction;
	bool interactioncomplete;
	bool paintingint;
	//<--Music-->
	/*void PlayMusic();
	bool b_musicSelected;
	bool b_inPM;
	bool b_checkinPM;
	bool b_inPC;
*/
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
		
		NUM_GEOMETRY,
	};
	enum OBJECT_TYPE
	{
		OBJ_PLAYER, // player
		OBJ_ROOMWALL1,
		OBJ_ROOMWALL2,
		OBJ_HOUSEWALL1,
		OBJ_HOUSEWALL2,
		OBJ_HOUSEWALL3,
		OBJ_HOUSEWALL4,
		OBJ_HOUSEWALL5,
		OBJ_HOUSEWALL6,
		OBJ_HOUSEWALL7,
		OBJ_HOUSEWALL8,
		OBJ_HOUSEWALL9,
		OBJ_HOUSEWALL10,
		OBJ_SOFA,
		OBJ_DOOR,
		OBJ_TABLE,
		OBJ_BOOKSHELF,
		OBJ_SAFE,
		OBJ_BED,
		OBJ_DOORINT,
		OBJ_KEY1,
		OBJ_SECRETWALL,
		OBJ_SECRETWALLINT,
		OBJ_PAINTINGCASTLE,
		OBJ_PAINTINGFOREST,
		OBJ_PAINTINGFRUIT,
		OBJ_TABLEPAINTING,
		OBJ_SWITCHONE,
		OBJ_SWITCHTWO,
		OBJ_PILLOW,
		OBJ_KEY2,
		OBJ_TVTABLEDRAWERINT,
		OBJ_SAFEINT,
		OBJ_ALL,//when adding add directly above 
		NUM_OBJ
	};
	MS modelStack, viewStack, projectionStack;

	Mesh* meshList[NUM_GEOMETRY];

	ObjectBox* Obj[NUM_OBJ];

	static Camera2 camera;

	Light light[2];

	void RenderMesh(Mesh *mesh, bool enableLight);

	void RenderText(Mesh* mesh, std::string text, Color color);

	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
};
#endif
