#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "PuzzleRoom.h"
#include "Utility.h"
#include <string>
#include "Sound.h"

float PuzzleRoom::lastX = 0.0f;
float PuzzleRoom::lastY = 0.0f;
Camera2 PuzzleRoom::camera = Camera2();

PuzzleRoom::PuzzleRoom()
{
	first = last = current = forward = backward = NULL;
}

void PuzzleRoom::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = (float)xpos - lastX;
	float yoffset = (float)ypos - lastY;
	float sensitivity = 0.05f;

	lastX = (float)xpos;
	lastY = (float)ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Vector3 view = camera.target - camera.position;
	Mtx44 rotate;
	rotate.SetToRotation(-xoffset, 0.0f, 1.0f, 0.0f);
	view = rotate * view;

	Vector3 rightVector = view.Cross(camera.up);
	rotate.SetToRotation(-yoffset, rightVector.x, rightVector.y, rightVector.z);
	view = rotate * view;

	camera.target = camera.position + view;
}

PuzzleRoom::~PuzzleRoom()
{
	delete PuzzleTimer;
}
void PuzzleRoom::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)
	PuzzleTimer = new StopWatchTimer;
	checkmodelStack = false;
	
	itemcollect = false;
	b_viewStats = false;

	// testing irrklan
	music::player.init();
	music::player.setSoundVol(0.5);
	// music::player.playSound("Sound//Scene1//PuzzleBGM1.wav", true);
	music::player.playSound("Sound//Scene1//PuzzleBGM2.wav", true);

	//<----for BMO body animation movement when running---->
	//LeftLegX = 90.0f;
	//RightLegX = 90.0f;
	//ArmRotation = 0.0f;
	//TranslateBodyX = 0.0f;
	//TranslateBodyY = 15.0f;
	//TranslateBodyZ = 0.0f;
	//RotateBody = 0.0f;

	//<--Music-->
	/*b_musicSelected = false;
	b_inPM = false;
	b_inPC = false;*/

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LSPEED = 30.0f;

	camera.Init(Vector3(-67.99, 52.75, -6.7), Vector3(-67.99, 52.8, -6.016), Vector3(0, 1, 0));

	currentCamPos = camera.position;
	currentCamTarget = camera.target;
	getCurrentCam = true;

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	//m_programID = LoadShaders("Shader//Shading.vertexshader","Shader//LightSource.fragmentshader");
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Blending.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	//Light 1
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	//light 2
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	
	glUseProgram(m_programID);

	light[0].type = Light::LIGHT_POINT; //light of the room
	light[0].position.Set(0.5, 89.75, 35.9);//position of light
	light[0].color.Set(1, 1, 1);//color that light emits
	light[0].power =1.0f;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	light[1].type = Light::LIGHT_POINT; //light of the room
	//light[1].position.Set(72.94, 80.76, 19);//position of light
	light[1].position.Set(0.5, 89.75, 35.9);
	light[1].color.Set(1, 1, 1);//color that light emits
	light[1].power = 1.0f;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
	
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	glUniform1i(m_parameters[U_NUMLIGHTS], 1);//if you add lights, add number here

	meshList[GEO_LIGHTBALL1] = MeshBuilder::GenerateSphere("Light Sphere", Color(1.0f, 1.0f, 1.0f), 18, 36, 1.0f, 360.0f);
	meshList[GEO_LIGHTBALL2] = MeshBuilder::GenerateSphere("Light Sphere", Color(1.0f, 1.0f, 1.0f), 18, 36, 1.0f, 360.0f);
	Obj[OBJ_PLAYER] = new ObjectBox(Vector3(camera.position.x, camera.position.y, camera.position.z), 5.f, 25.f, 5.f);
	//Guide lines - Turn on if need
	//meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Reference", 1000.0f, 1000.0f, 1000.0f);
	//walls
	meshList[GEO_HOUSE] = MeshBuilder::GenerateOBJ("House" ,"OBJ//house.obj");
	meshList[GEO_HOUSE]->textureID = LoadTGA("Image//house.tga");
	Obj[OBJ_HOUSEWALL1] = new ObjectBox(Vector3(-21.35,58.50,-12.51),117.23,65,1);
	Obj[OBJ_HOUSEWALL2] = new ObjectBox(Vector3(-31.35, 58.50, 81.51),117.23,65,1);
	Obj[OBJ_HOUSEWALL3] = new ObjectBox(Vector3(-99.99, 58.50, 33.01),1 , 65, 98.39);
	Obj[OBJ_HOUSEWALL4] = new ObjectBox(Vector3(106.99, 58.50, 33.01), 1, 65, 98.39);
	Obj[OBJ_HOUSEWALL5] = new ObjectBox(Vector3(1.5, 58.50,-84 ), 1, 65, 90.295);
	Obj[OBJ_HOUSEWALL6] = new ObjectBox(Vector3(106.37, 58.50,-68 ), 1, 65, 90.295);
	Obj[OBJ_HOUSEWALL7] = new ObjectBox(Vector3(81.62, 58.50, -105.94), 43.49, 65, 1);
	Obj[OBJ_HOUSEWALL8] = new ObjectBox(Vector3(105, 58.50, 116.25), 1, 65,57.49);
	Obj[OBJ_HOUSEWALL9] = new ObjectBox(Vector3(30.1, 58.50, 116.25), 1, 65,57.49);
	Obj[OBJ_HOUSEWALL10] = new ObjectBox(Vector3(68, 58.50, 147), 76, 65, 1);

	meshList[GEO_ROOM1WALL] = MeshBuilder::GenerateOBJ("Room1Wall", "OBJ//room1walls.obj");
	meshList[GEO_ROOM1WALL]->textureID = LoadTGA("Image//woodwall.tga");
	Obj[OBJ_ROOMWALL1] = new ObjectBox(Vector3(26.56f, 58.50f, -20.8f), 21.85f, 84.9f, 10.66);
	Obj[OBJ_ROOMWALL2] = new ObjectBox(Vector3(96.56f, 58.50f, -20.8f), 50.52f, 84.9f, 10.66);

	meshList[GEO_DOORTOROOM1] = MeshBuilder::GenerateOBJ("Room1Wall", "OBJ//doortoroom12.obj");
	meshList[GEO_DOORTOROOM1]->textureID = LoadTGA("Image//DoorTextures.tga");
	Obj[OBJ_DOORINT] = new ObjectBox(Vector3(55.7, 58.50, -18.5), 41.82, 65, 55);

	meshList[GEO_SOFA] = MeshBuilder::GenerateOBJ("Sofa", "OBJ//sofa2.obj");
	meshList[GEO_SOFA]->textureID = LoadTGA("Image//sofatxt.tga");
	Obj[OBJ_SOFA] = new ObjectBox(Vector3(39, 58.50, 37), 19.5, 65, 40);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_PAINTINGFRUIT] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//fruitpainting.obj");
	meshList[GEO_PAINTINGFRUIT]->textureID = LoadTGA("Image//fruitpainting.tga");
	Obj[OBJ_PAINTINGFRUIT] = new ObjectBox(Vector3(10.8, 52.50, -7.3), 16, 36, 10);

	meshList[GEO_PAINTINGCASTLE] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//castlepainting.obj");
	meshList[GEO_PAINTINGCASTLE]->textureID = LoadTGA("Image//castlepainting.tga");
	Obj[OBJ_PAINTINGCASTLE] = new ObjectBox(Vector3(-39.3, 52.50, 75), 16, 36, 10);

	meshList[GEO_PAINTINGKEY] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//keypainting.obj");
	meshList[GEO_PAINTINGKEY]->textureID = LoadTGA("Image//keypainting.tga");

	meshList[GEO_PAINTINGFOREST] = MeshBuilder::GenerateOBJ("forestpainting", "OBJ//forestpainting.obj");
	meshList[GEO_PAINTINGFOREST]->textureID = LoadTGA("Image//forestpainting.tga");
	Obj[OBJ_PAINTINGFOREST] = new ObjectBox(Vector3(-31.9, 52.50, -2.37), 16, 36, 10);

	meshList[GEO_DOORENTRANCE] = MeshBuilder::GenerateOBJ("doorentrance", "OBJ//doorentrance.obj");
	meshList[GEO_DOORENTRANCE]->textureID = LoadTGA("Image//DoorTextures.tga");
	
	meshList[GEO_TV] = MeshBuilder::GenerateOBJ("TV", "OBJ//tv.obj");
	meshList[GEO_TV]->textureID = LoadTGA("Image//TVTextures.tga");

	meshList[GEO_TVTABLE] = MeshBuilder::GenerateOBJ("TV", "OBJ//tvtable.obj");
	meshList[GEO_TVTABLE]->textureID=LoadTGA("Image//TVTableTextures.tga");
	Obj[OBJ_TABLE] = new ObjectBox(Vector3(94.5, 58.50, 35.7), 7, 65, 19.1);


	meshList[GEO_TVTABLEDRAWER] = MeshBuilder::GenerateOBJ("TVTableDrawer", "OBJ//tvtabledrawer.obj");
	meshList[GEO_TVTABLEDRAWER]->textureID = LoadTGA("Image//TVTableDrawerTextures.tga");
	Obj[OBJ_TVTABLEDRAWERINT] = new ObjectBox(Vector3(80.5, 58.50, 35.7), 7, 65, 19.1);


	meshList[GEO_BOOKSHELF] = MeshBuilder::GenerateOBJ("BookShelve", "OBJ//bookshelf.obj");
	meshList[GEO_BOOKSHELF]->textureID = LoadTGA("Image//BookShelfTexture.tga");
	Obj[OBJ_BOOKSHELF] = new ObjectBox(Vector3(90.23, 58.50, -37.04),29.08, 65, 8.49);
	Obj[OBJ_SECRETWALLINT] = new ObjectBox(Vector3(90.23, 58.50, -37.04), 29.08, 65, 28.49);


	meshList[GEO_BOOK1] = MeshBuilder::GenerateOBJ("Book1", "OBJ//book1.obj");
	meshList[GEO_BOOK1]->textureID = LoadTGA("Image//Red.tga");

	meshList[GEO_BOOK2] = MeshBuilder::GenerateOBJ("Book2", "OBJ//book2.obj");
	meshList[GEO_BOOK2]->textureID = LoadTGA("Image//Green.tga");

	meshList[GEO_BOOK3] = MeshBuilder::GenerateOBJ("Book3", "OBJ//book3.obj");
	meshList[GEO_BOOK3]->textureID = LoadTGA("Image//Blue.tga");

	meshList[GEO_BOOK4] = MeshBuilder::GenerateOBJ("Book1", "OBJ//book4.obj");
	meshList[GEO_BOOK4]->textureID = LoadTGA("Image//Purple.tga");

	meshList[GEO_BOOKBLACK] = MeshBuilder::GenerateOBJ("Book1", "OBJ//blackbook.obj");
	meshList[GEO_BOOKBLACK]->textureID = LoadTGA("Image//Black.tga");

	meshList[GEO_MATTRESS] = MeshBuilder::GenerateOBJ("Book1", "OBJ//mattress.obj");
	meshList[GEO_MATTRESS]->textureID = LoadTGA("Image//Mattress.tga");

	meshList[GEO_BEDFRAME] = MeshBuilder::GenerateOBJ("Book1", "OBJ//bedframe.obj");
	meshList[GEO_BEDFRAME]->textureID = LoadTGA("Image//BedFrame.tga");
	Obj[OBJ_BED] = new ObjectBox(Vector3(34, 58.50, -104.03), 40.03, 65, 26.82);


	meshList[GEO_PILLOW] = MeshBuilder::GenerateOBJ("Book1", "OBJ//pillow.obj");
	meshList[GEO_PILLOW]->textureID = LoadTGA("Image//Pillow.tga");
	if (pillowmoved == false)
	{
		Obj[OBJ_PILLOW] = new ObjectBox(Vector3(21, 58.50, -86.03), 6, 65, 34.82);
	}
	


	meshList[GEO_SECRETWALL] = MeshBuilder::GenerateOBJ("Book1", "OBJ//secretwall.obj");
	meshList[GEO_SECRETWALL]->textureID = LoadTGA("Image//SecretWall.tga");



	meshList[GEO_SAFE] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safebox.obj");
	meshList[GEO_SAFE]->textureID = LoadTGA("Image//SafeBox.tga");
	Obj[OBJ_SAFE] = new ObjectBox(Vector3(110.55, 58.50, 135.23), 48, 65, 48);
	Obj[OBJ_SAFEINT] = new ObjectBox(Vector3(110.55, 58.50, 135.23), 51, 65, 51);


	meshList[GEO_SAFEDOOR] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safedoor.obj");
	meshList[GEO_SAFEDOOR]->textureID = LoadTGA("Image//SafeDoor.tga");

	meshList[GEO_KEY1] = MeshBuilder::GenerateOBJ("Key1,", "OBJ//key1.obj");
	meshList[GEO_KEY1]->textureID = LoadTGA("Image//Key1.tga");
	Obj[OBJ_KEY1] = new ObjectBox(Vector3(10, 52.50, 76), 16, 36, 10);

	//KeyNote
	meshList[GEO_KEY1NOTE] = MeshBuilder::GenerateQuad("afs", Color(1, 1, 1), 1, 1, 1);
	meshList[GEO_KEY1NOTE]->textureID = LoadTGA("Image//keyonewords.tga");

	meshList[GEO_TABLEPAINTING] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//tablepainting.obj");
	meshList[GEO_TABLEPAINTING]->textureID = LoadTGA("Image//TablePainting.tga");
	Obj[OBJ_TABLEPAINTING] = new ObjectBox(Vector3(-0.95, 58.50, 65.455), 33.52, 65, 28.8);

	meshList[GEO_SWITCHONE] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//switchone.obj");
	meshList[GEO_SWITCHONE]->textureID = LoadTGA("Image//Pillow.tga"); 
	Obj[OBJ_SWITCHONE] = new ObjectBox(Vector3(-77.8, 58.50, -15.9), 3, 65, 9.0);

																			  
	meshList[GEO_SWITCHTWO] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//switchtwo.obj");
	meshList[GEO_SWITCHTWO]->textureID = LoadTGA("Image//Pillow.tga");
	Obj[OBJ_SWITCHTWO] = new ObjectBox(Vector3(1.8, 58.50, -54), 9, 65, 9.0);

	meshList[GEO_KEY2] = MeshBuilder::GenerateOBJ("Key1,", "OBJ//key2.obj");
	meshList[GEO_KEY2]->textureID = LoadTGA("Image//Key1.tga");
	Obj[OBJ_KEY2] = new ObjectBox(Vector3(21, 58.50, -86.03), 6, 65, 34.82);

	// Patient Obj
	meshList[GEO_HAIR] = MeshBuilder::GenerateOBJ("Patient", "OBJ//Hair.obj");
	meshList[GEO_HAIR]->textureID = LoadTGA("Image//Hair.tga");

	meshList[GEO_FACE] = MeshBuilder::GenerateOBJ("Patient", "OBJ//Face.obj");
	meshList[GEO_FACE]->textureID = LoadTGA("Image//Face.tga");

	meshList[GEO_BODY] = MeshBuilder::GenerateOBJ("Patient", "OBJ//Body.obj");
	meshList[GEO_BODY]->textureID = LoadTGA("Image//Chest.tga");

	meshList[GEO_RARM] = MeshBuilder::GenerateOBJ("Patient", "OBJ//RArm.obj");
	meshList[GEO_RARM]->textureID = LoadTGA("Image//Body.tga");

	meshList[GEO_LARM] = MeshBuilder::GenerateOBJ("Patient", "OBJ//LArm.obj");
	meshList[GEO_LARM]->textureID = LoadTGA("Image//Body.tga");

	meshList[GEO_RHAND] = MeshBuilder::GenerateOBJ("Patient", "OBJ//RHand.obj");
	meshList[GEO_RHAND]->textureID = LoadTGA("Image//Hand.tga");

	meshList[GEO_LHAND] = MeshBuilder::GenerateOBJ("Patient", "OBJ//LHand.obj");
	meshList[GEO_LHAND]->textureID = LoadTGA("Image//Hand.tga");

	meshList[GEO_RLEG] = MeshBuilder::GenerateOBJ("Patient", "OBJ//RLeg.obj");
	meshList[GEO_RLEG]->textureID = LoadTGA("Image//Body.tga");

	meshList[GEO_LLEG] = MeshBuilder::GenerateOBJ("Patient", "OBJ//LLeg.obj");
	meshList[GEO_LLEG]->textureID = LoadTGA("Image//Body.tga");

	meshList[GEO_CROTCH] = MeshBuilder::GenerateOBJ("Patient", "OBJ//Crotch.obj");
	meshList[GEO_CROTCH]->textureID = LoadTGA("Image//Body.tga");

	Obj[OBJ_PATIENT] = new ObjectBox(Vector3(57, 36.50, 45),17, 20, 27);
	Obj[OBJ_PATIENTINT] = new ObjectBox(Vector3(57, 46.50, 45), 27, 20, 37);

	//Final Door
	Obj[OBJ_LASTDOOR] = new ObjectBox(Vector3(-59.5, 46.50, -11.48), 25, 20, 5);


	//InteractCheck
	Obj[OBJ_ALL] = new ObjectBox(Vector3(0,0,0), 1000, 1000, 1000);


	/*meshList[GEO_KEY]->material.kAmbient.Set(0.00f, 0.00f, 0.00f);
	meshList[GEO_KEY]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_KEY]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
 */ 
	meshList[GEO_KEY1]->material.kShininess = 1.0f;

	// User Interface -> Raphael Added

	meshList[GEO_START] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 25, 20, 0);
	meshList[GEO_START]->textureID = LoadTGA("Image//Stage1.tga");
}

//void PuzzleRoom::PlayMusic()
//{
//	
//}

void PuzzleRoom::Update(double dt)
{
	
	// Raphael Added
	score = score + 0.2;
	if (score > 20)
	{
		showIntro = false;
	}

	// Raphael Added -> not working 
	if (camera.position.x > 80 && camera.position.x < 90 
		&& camera.position.z > 25 && camera.position.z < 45)
	{ 
		music::player.init();
		music::player.setSoundVol(0.3);
		music::player.playSound("Sound//Scene1//Static2.wav");
	}


	elapsedtime += dt;
	//Timer
	//If timer reach 0
	if (PuzzleTimer->d_GetPuzzleSceneTime() <= 0)
	{
		timerunout = true;
	}
	//Prevent time from going negative
	if (timerunout == false)
	{
		PuzzleTimer->v_UpdateTime(dt);
	}
	if (timeleft == true)
	{
		//Application::timerh = PuzzleTimer->d_GetTimeLeftAfterPuzzle();
	}
	//Inventory
	if (Application::IsKeyPressed(VK_LEFT)&& elapsedtime >1)
	{
		if (itemcollect ==true)
		{
			elapsedtime = 0;
			printPrev();
		}
	}
	if(Application::IsKeyPressed(VK_RIGHT) && elapsedtime > 1)
	{
		if (itemcollect == true)
		{
			elapsedtime = 0;
			printNext();
		}
	}
	//DoorOpening
	if (doorint == true && havekey1 == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			
			interactioncomplete = true;
			if (elapsedtime > bouncetime)
			{

				door1open = !door1open;
				bouncetime = elapsedtime + 0.4f;

				music::player.init();
				music::player.setSoundVol(0.5);
				music::player.playSound("Sound//Scene1//UnlockDoor2.wav");
			}

		}
	}
	if(doorint == true && havekey1 == false || drawerint == true && havekey2 == false)
	{
		if (Application::IsKeyPressed('E'))
		{
			lockeddoortext = true;

			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene1//MouseClick.wav");
		}
	}
	
	if (door1open && RotateDoor1< 85)
	{
		Obj[OBJ_DOOR] = new ObjectBox(Vector3(55.7, 58.50, -16.5), 31.82, 65, 7);
		RotateDoor1 += 35 * dt * 2;
	}
	else if (!door1open && RotateDoor1 > 0)
	{
		Obj[OBJ_DOOR] = new ObjectBox(Vector3(69, 58.50, -32.28), 7, 65, 30);
		RotateDoor1 -= 35 * dt * 2;
	}
	//SecretDoorOpening
	if (secretdoorint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;
			if (elapsedtime > bouncetime)
			{
				secretdooropen = !secretdooropen;
				bouncetime = elapsedtime + 0.4f;

				music::player.init();
				music::player.setSoundVol(0.5);
				music::player.playSound("Sound//Scene1//MouseClick.wav");
			}
		}

	}
	if (secretdooropen && secretdoortranslation < 30)
	{
		Obj[OBJ_SECRETWALL] = new ObjectBox(Vector3(68.7, 58.50, 81.69), 83.5, 70, 2);
		secretdoortranslation += 10 * dt * 2;
	}
	else if (!secretdooropen && secretdoortranslation> 0)
	{
		Obj[OBJ_SECRETWALL] = new ObjectBox(Vector3(68.7, 158.50, 81.69), 83.5, 70, 2);
		secretdoortranslation -= 10 * dt * 2;
	}
	//TvTableDrawer
	if (draweropen == false && havekey2 == true && drawerint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;

		}
		if (draweropen == false && drawertranslation <= 1.5 && interactioncomplete == true)
		{
			drawertranslation += 0.5 * dt;
			if (drawertranslation >=0.5)
			{
				draweropen = true;
			}
		}
	}
	//Pillow
	if (pillowmoved == false && pillowint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;

		}
		if (pillowmoved == false && pillowtranslation <=3 && interactioncomplete == true)
		{
			pillowtranslation += 3 * dt ;
			if (pillowtranslation >= 3)
			{
				pillowmoved = true;
			}
		}
	}
	if ( pillowmoved == true)
	{
		Obj[OBJ_PILLOW] = new ObjectBox(Vector3(48, 58.50, -86.03), 6, 65, 34.82);
	}
	if (Application::IsKeyPressed('G')&& elapsedtime >1)
	{
		elapsedtime = 0;
		havekey1 = true;
		havekey2 = true; 
		itemcollect = true;
		meshList[GEO_KEY1] = MeshBuilder::GenerateQuad("Key1", Color(1, 1, 1), 1, 1, 0);
		meshList[GEO_KEY1]->textureID = LoadTGA("Image//InvKey1.tga");
		meshList[GEO_KEY1]->textureID = LoadTGA("Image//keyoneword");
		uploadItem(27);
		std::cout << "0" << std::endl;
	}
	//safe
	if (safeint == true && safeopen == false)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;
			safecracking = true;
			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene1//MouseClick.wav");
		}
	}
	if (safecracking == true)
	{
		other = false;
		if (Application::IsKeyPressed('8'))
		{
			eight = true;
			other = false;
			six = false;
			one = false;
			two = false;
		}
		else if (Application::IsKeyPressed('6') && eight == true)
		{
			six = true;
			one = false;
			two = false;
		}
		else if (Application::IsKeyPressed('1') && six == true)
		{
			one = true;
			two = false;
		}
		else if (Application::IsKeyPressed('2') && one == true)
		{
			two = true;
		}
		else if (Application::IsKeyPressed('E'))
		{

		}
		else if (Application::IsKeyPressed('3')&& Application::IsKeyPressed('4')&& Application::IsKeyPressed('5') && Application::IsKeyPressed('7') && Application::IsKeyPressed('9') && Application::IsKeyPressed('0'))
		{
			other = true;
			eight = false;
			six = false;
			one = false;
			two = false;
		}

	}
	if (eight == true && six == true && one == true && two == true)
	{
		codecracked = true;
	}
	if (safeopen == false && safeint == true &&codecracked == true)
	{
			safeopen = true;
			havekey3 = true;
	}
	if (safeopen == true)
	{

	}
	if (havekey3 == true)
	{
		eight = false;
		six = false;
		one = false;
		two = false;

	}
	//key2
	if (key2int == true && pillowmoved == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;
			havekey2 = true;

			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene1//PickUp.wav");
		}
	}
	//key1
	if (key1int == true)
	{
		if (Application::IsKeyPressed('E') && elapsedtime > 1)
		{
			interactioncomplete = true;
			havekey1 = true;
			itemcollect = true;
			meshList[GEO_KEY1] = MeshBuilder::GenerateQuad("twst", Color(1, 1, 1), 1, 1, 1);
			meshList[GEO_KEY1]->textureID = LoadTGA("Image//InvKey1.tga");
			uploadItem(27);
			meshList[GEO_KEY1NOTE] = MeshBuilder::GenerateQuad("twst",Color(1, 1, 1), 1, 1, 1);
			meshList[GEO_KEY1NOTE]->textureID = LoadTGA("Image//keyonewords.tga");
			uploadItem(43);
			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene1//PickUp.wav");
		}
	}
	//Patient
	if (patientint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete =true;
			patienthint = true;

			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene1//MouseClick.wav");
		}
	}
	if (havekey3 == true && patientint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			havepatient = true;
		}
	}
	if (havepatient == true)
	{
		doorunlocked = true;
	}
	if (doorunlocked == true && finaldoorint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			timeleft = true;
			interactioncomplete = true;
			Application app;
			app.SetSceneNumber(2);
			app.Run();
		}
	}
	if (doorunlocked == false && finaldoorint == true)
	{
		if (Application::IsKeyPressed('E'))
		{
			interactioncomplete = true;
			lockeddoortext = true;	
		}
	}
	
	////switchone
	//if(switchoneint == true)
	//{
	//	if (light[0].power >= 1.0f)
	//	{
	//		lightoneon = true;
	//	}
	//	else
	//	{
	//		lightoneon = false;
	//	}
	//	if (Application::IsKeyPressed('E') && elapsedtime > bouncetime)
	//	{
	//		bouncetime = elapsedtime += 0.4f;
	//		interactioncomplete = true;
	//		if (lightoneon == false)
	//		{
	//			light[0].power = 10.0f;
	//			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);

	//		}
	//		else
	//		{
	//			light[0].power = 0.f;
	//			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);

	//		}

	//	}
	//}
	////switchtwo
	//if (switchtwoint == true)
	//{
	//	if (light[1].power >= 1.0f)
	//	{
	//		lighttwoon = true;
	//	}
	//	else
	//	{
	//		lighttwoon = false;
	//	}
	//	if (Application::IsKeyPressed('E'))
	//	{
	//		interactioncomplete = true;
	//		if (lighttwoon == false)
	//		{
	//			light[1].power = 10.0f;
	//			glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);

	//		}
	//		else
	//		{
	//			light[1].power = 0.f;
	//			glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);

	//		}
	//	}
	//}


	if (Application::IsKeyPressed('5'))
	{
		music::player.stopSound(); // end all music at the des of scene
		
		Application app;
		app.SetSceneNumber(2);
		app.Run();
	}
	if (Application::IsKeyPressed('3'))
	{

	}
	if (Application::IsKeyPressed('4'))
	{

	}
	/*if (Application::IsKeyPressed('6'))
	{
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('7'))
	{
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed('8'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (Application::IsKeyPressed('9'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}*/
	

	if (Application::IsKeyPressed('P'))
		b_viewStats = true;
	else
		b_viewStats = false;

	fps = 1.0f / (float)dt;

	if (Application::IsKeyPressed('Z'))
	{
		light[0].type = Light::LIGHT_POINT;  // For a lamp post
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to POINT and pass the information to shader
	}
	else if (Application::IsKeyPressed('X'))
	{
		light[0].type = Light::LIGHT_DIRECTIONAL; // Used for smt like the sun, somewhere so far it shines on everything depending on angle
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to DIRECTIONAL and pass the information to shader
	}
	else if (Application::IsKeyPressed('C'))
	{
		light[0].type = Light::LIGHT_SPOT; // For a torch light
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to SPOT and pass the information to shader
	}
	
	Obj[OBJ_PLAYER]->setOBB(Vector3(camera.position.x, camera.position.y, camera.position.z));
	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[AllObjs]))
		{
			//Doorint
			if (AllObjs == 19)
			{
				doorint = true;
				interaction = true;
				break;
			}
			//Key1
			if (AllObjs == 20)
			{
				key1int = true;
				interaction = true;
				break;
			}
			//Secretwallint
			if (AllObjs == 22)
			{
				
				secretdoorint = true;
				interaction = true;
				break;
			}
			//paintings
			if (AllObjs >= 23 && AllObjs <= 25)
			{
				paintingint = true;
				interaction = true;
				break;
			}
			//LightSwitchone
			if(AllObjs ==27)
			{
				switchoneint = true;
				interaction = true;
				break;
			}
			//LightSwitchtwo
			if (AllObjs == 28)
			{
				switchtwoint =true;
				interaction = true;
				break;
			}
			//Pillow
			if (AllObjs == 29)
			{
				pillowint = true;
				interaction = true;
				break;
			}
			//Key2
			if (AllObjs == 30)
			{
				key2int = true;
				interaction = true;
				break;
			}
			//TableDrawer
			if (AllObjs == 31)
			{
				drawerint = true;
				interaction = true;
				break;
			}
			//Safe
			if (AllObjs == 32)
			{
				safeint = true;
				interaction = true;
				break;
			}
			//PatientInteraction
			if (AllObjs == 34)
			{
				patientint = true;
				interaction = true;
				break;
			}
			//FinalDoor
			if (AllObjs == 35)
			{
				finaldoorint = true;
				interaction = true;
				break;
			}
			if (AllObjs == NUM_OBJ-1)
			{
				finaldoorint = false;
				safeint = false;
				patienthint = false;
				patientint = false;
				drawerint = false;
				key2int = false;
				pillowint = false;
				switchoneint = false;
				switchtwoint = false;
				paintingint = false;
				lockeddoortext = false;
				doorint = false;
				key1int = false;
				secretdoorint =false;
				interaction = false;
				interactioncomplete = false;
				other = false;
				break;
			}
			collide = true;
			//camera.position = currentCamPos;
			//camera.target = currentCamTarget;
			camera.position.x = currentCamPos.x;
			camera.position.z = currentCamPos.z;
			camera.target = currentCamTarget;
			break;
		}
		collide = false;
	}
	
	if (!collide)
	{
		//currentCamPos = camera.position;
		//currentCamTarget = camera.target;
		currentCamPos = camera.position.x;
		currentCamPos = camera.position.z;
		currentCamTarget = camera.target;
	}
	if (getCurrentCam)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}
	//PlayMusic();
	camera.Update(dt,true);
}

void PuzzleRoom::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderSkybox();
	CreepyHouse();

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() *
			light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1,
			&spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightPosition_cameraspace.x);
	}
	//light 1
	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightDirection_cameraspace.x);
	}
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() *
			light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1,
			&spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1,
			&lightPosition_cameraspace.x);
	}

	////<-----------Axes----------->
	//modelStack.PushMatrix();
	//RenderMesh(meshList[GEO_AXES], false);
	//modelStack.PopMatrix();

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL1], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL2], false);
	modelStack.PopMatrix();

	if (b_viewStats)
	{
		//<--FPS-->
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("FPS:" + std::to_string(fps)), Color(0, 0, 0), 2, 52, 58);
		modelStack.PopMatrix();
	}
	else
	{
		
	}
	//float time = 120;
	//<--Get cameras position-->
	/*modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT],("Time"+ std::to_string(PuzzleTimer->d_GetPuzzleSceneTime())),Color(0, 1, 0), 2, 25, 25);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Pos X:" + std::to_string(camera.position.x)+", Y:"+ std::to_string(camera.position.y) +" , Z:"+ std::to_string(camera.position.z)), Color(0, 1, 0), 2, 2, 5);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Tar X:" + std::to_string(camera.target.x)+", Y:"+ std::to_string(camera.target.y) +" , Z:"+ std::to_string(camera.target.z)), Color(1, 0, 0), 2, 2, 7);
	 modelStack.PopMatrix();*/
	 if (interaction == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Press E to interact"), Color(1, 1, 1), 2, 14, 48);
		 modelStack.PopMatrix();
	 }
	 if (interactioncomplete == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Press E to interact"), Color(1, 0, 0), 2, 14, 48);
		 modelStack.PopMatrix();
	 }
	
	 if (lockeddoortext == true )
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Door is locked"), Color(1, 1, 1), 2, 14, 42);
		 modelStack.PopMatrix();
	 }
	 if (paintingint == true)
	 {
		 if(Application::IsKeyPressed('E'))
		 { 
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Nice Painting"), Color(1, 1, 1), 2, 4, 42);
		 modelStack.PopMatrix();}
	 }
	 if (havekey1 == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Unlocks a door perhaps"), Color(1, 1, 1), 1.5, 30, 16);
		 modelStack.PopMatrix();
		 rendertag();
	 }
	 if (havekey2 == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("You Have Key 2"), Color(1, 1, 1), 2, 0, 43);
		 modelStack.PopMatrix();
	 }
	 if (havekey3 == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("You Have Final Key "), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (draweropen == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("You Found A Note with 8  6  1  2 "), Color(1, 1, 1), 2, 4, 46);
		 modelStack.PopMatrix();
	 }
	 if (patienthint == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("3 Keys , 1 in a thousand words"), Color(1, 1, 1), 2, 4, 46.2);
		 RenderTextOnScreen(meshList[GEO_TEXT], ("	     , 1 is a thousand pages"), Color(1, 1, 1), 2, 4, 45);
		 RenderTextOnScreen(meshList[GEO_TEXT], ("	     , 1 under what u watch"), Color(1, 1, 1), 2, 4, 43.8);
		 modelStack.PopMatrix();					      
	 }
	 if(other == true)
	 { 
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("Wrong Code Try Again "), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (eight == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("8"), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (six == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], (" 6"), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (one == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("  1"), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (two == true)
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("   2"), Color(1, 1, 1), 2, 4, 44);
		 modelStack.PopMatrix();
	 }
	 if (safeint == true && eight == false && havekey3== false )
	 {
		 modelStack.PushMatrix();
		 RenderTextOnScreen(meshList[GEO_TEXT], ("ENTER THE PASSCODE"), Color(1, 1, 1), 2, 1, 44);
		 modelStack.PopMatrix();
	 }

	 if (showIntro) // Raphael Added
	 {
		 modelStack.PushMatrix();
		 DrawHUD(meshList[GEO_START], Color(0, 0, 1), false, 1, 40, 30);
		 modelStack.PopMatrix();
	 }
}

void PuzzleRoom::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
			&modelView_inverse_transpose.a[0]);
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

static const float SKYBOXSIZE = 300.f;

void PuzzleRoom::RenderSkybox()
{

}

void PuzzleRoom::CreepyHouse()
{
	//ALL MUST BE BELOW THIS
	//Scale
	modelStack.PushMatrix();
	modelStack.Scale(10.f,10.f,10.f);
	modelStack.PushMatrix();
	if (havepatient == false)
	{
		modelStack.Translate(5.5, 2.8, 5.5);
		modelStack.Scale(0.7, 0.7, 0.7);
		//modelStack.Rotate(270, 0, 0, 1);
		//modelStack.Rotate(270, 0, 1, 0);
		modelStack.Rotate(270, 1, 0, 0);
		RenderMesh(meshList[GEO_HAIR], true);
		RenderMesh(meshList[GEO_FACE], true);
		RenderMesh(meshList[GEO_BODY], true);
		RenderMesh(meshList[GEO_RARM], true);
		RenderMesh(meshList[GEO_LARM], true);
		RenderMesh(meshList[GEO_RHAND], true);
		RenderMesh(meshList[GEO_LHAND], true);
		RenderMesh(meshList[GEO_RLEG], true);
		RenderMesh(meshList[GEO_LLEG], true);
		RenderMesh(meshList[GEO_CROTCH], true);
		modelStack.PopMatrix();
	}
	//Key2
	if (havekey2 == false)
	{
		RenderMesh(meshList[GEO_KEY2], true);
	}
	//Switches
	RenderMesh(meshList[GEO_SWITCHONE], true);
	RenderMesh(meshList[GEO_SWITCHTWO], true);

	//TablePainting
	RenderMesh(meshList[GEO_TABLEPAINTING], true);
	//key
	if (havekey1 == false)
	{
		RenderMesh(meshList[GEO_KEY1], true);
	}
	//Safe
	RenderMesh(meshList[GEO_SAFE], true);
	RenderMesh(meshList[GEO_SAFEDOOR], true);
	//SecretWall
	modelStack.PushMatrix();
	modelStack.Translate(0,-30+secretdoortranslation,0);
	RenderMesh(meshList[GEO_SECRETWALL], true);
	modelStack.PopMatrix();


	//BedSet
	RenderMesh(meshList[GEO_MATTRESS], true);
	RenderMesh(meshList[GEO_BEDFRAME], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0 + pillowtranslation,0,0);
	RenderMesh(meshList[GEO_PILLOW], true);
	modelStack.PopMatrix();

	//Copied books
	for (int i = 1; i < 18; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0 - i*0.1  ,0,0);
		//Book1
		 RenderMesh(meshList[GEO_BOOK1], true);
		//Book2	
		RenderMesh(meshList[GEO_BOOK2], true);
		//Book3
		RenderMesh(meshList[GEO_BOOK3], true);
		//Book4
		RenderMesh(meshList[GEO_BOOK4], true);
		modelStack.PopMatrix();
	}
	//Book1
	RenderMesh(meshList[GEO_BOOK1], true);
	//Book2	
	RenderMesh(meshList[GEO_BOOK2], true);
	//Book3
	RenderMesh(meshList[GEO_BOOK3], true);
	//Book4
	RenderMesh(meshList[GEO_BOOK4], true);
	//BookBlack
	RenderMesh(meshList[GEO_BOOKBLACK], true);
	//BookShelfe
	RenderMesh(meshList[GEO_BOOKSHELF], true);
	//TV
	RenderMesh(meshList[GEO_TV], true);
	//TvTableDrawer
	modelStack.PushMatrix();
	modelStack.Translate(0-drawertranslation,0,0);
	RenderMesh(meshList[GEO_TVTABLEDRAWER], true);
	modelStack.PopMatrix();
	//Tvtable
	RenderMesh(meshList[GEO_TVTABLE], true);
	//doorentrance
	RenderMesh(meshList[GEO_DOORENTRANCE], true);
	//forestpainting
	RenderMesh(meshList[GEO_PAINTINGFOREST], true);
	//keypainting
	RenderMesh(meshList[GEO_PAINTINGKEY], true);
	//castlepainting
	RenderMesh(meshList[GEO_PAINTINGCASTLE], true);
	//fruitpainting
	RenderMesh(meshList[GEO_PAINTINGFRUIT],true);

	//Sofa
	RenderMesh(meshList[GEO_SOFA], true);
	//Room1Door
	modelStack.PushMatrix(); 
	modelStack.Translate(7.93, 5, -2.08);
	modelStack.Rotate(-90, 0.f, 1.f, 0.f);
	modelStack.Rotate(RotateDoor1, 0, 1, 0);
	modelStack.Translate(-7.93, -5, 2.08);
	RenderMesh(meshList[GEO_DOORTOROOM1],true);
	modelStack.PopMatrix();
	
	//RoomWall1
	RenderMesh(meshList[GEO_ROOM1WALL], true);


	//house
	RenderMesh(meshList[GEO_HOUSE], true);

	modelStack.PopMatrix();

}

void PuzzleRoom::RenderButton(int geo_circle, int geo_cylinder)
{
	
}

void PuzzleRoom::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0);
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void PuzzleRoom::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}
void PuzzleRoom::DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
			&modelView_inverse_transpose.a[0]);
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}
void PuzzleRoom::uploadItem(int newobject)
{
	forward = new Item(newobject);
	if (first == NULL)
	{
		first = last = forward;
		current = first;
	}
	else
	{
		last->next = forward;
		forward->prev = last;
		last = forward;
	}
	itemcount = 1;
	totalitem++;
}
void PuzzleRoom::printNext()
{
	Item *check;
	check = current->next;
	if (check != NULL)
	{
		current = check;
		itemcount++;
		rendertag();
	}
	else if (check == NULL && current == first)
	{
		current = last;
		itemcount = 1;
		rendertag();
	}
	else if (check == NULL && current == last)
	{
		current = first;
		itemcount = 1;
		rendertag();
	}
}
void PuzzleRoom::printPrev()
{
	Item *check;
	check = current->prev;
	if (check != NULL)
	{
		current = check;
		itemcount--;
		rendertag();
	}
	else if (check == NULL && current == first)
	{
		current = last;
		itemcount = 1;
		rendertag();
	}
	else if (check == NULL && current == last)
	{
		current = first;
		itemcount = 1;
		rendertag();
	}
}
void PuzzleRoom::rendertag()
{
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(itemcount) + "/" + std::to_string(totalitem), Color(0, 0, 1), 2, 5, 5);
	for (int i = 0; i < 34; i++)
	{
		if (current->data == i)
		{
			DrawHUD(meshList[i], Color(0, 0, 0), false, 5, 8, 2);
		}
		if (current->prev != NULL && current->next != NULL)
		{
			if (current->prev->data == i)
			{
				DrawHUD(meshList[i], Color(0, 0, 0), false, 5, 5, 2);
			}
			if (current->next->data == i)
			{
				DrawHUD(meshList[i], Color(0, 0, 0), false, 5, 11, 2);
			}
		}
		else if (current->prev == NULL && current->next != NULL)
		{
			if (current->next->data == i)
			{
				DrawHUD(meshList[i], Color(0, 0, 0), false, 5, 11, 2);
			}
		}
		else if (current->prev != NULL && current->next == NULL)
		{
			if (current->prev->data == i)
			{
				DrawHUD(meshList[i], Color(0, 0, 0), false, 5, 5, 2);

			}
		}
	}
}
void PuzzleRoom::Exit()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

	// Cleanup VBO here
}