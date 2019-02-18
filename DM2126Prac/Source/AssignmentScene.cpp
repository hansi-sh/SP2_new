#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "AssignmentScene.h"
#include "Utility.h"
#include <string>

float AssignmentScene::lastX = 0.0f;
float AssignmentScene::lastY = 0.0f;
Camera2 AssignmentScene::camera = Camera2();

AssignmentScene::AssignmentScene()
{
}

void AssignmentScene::mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

AssignmentScene::~AssignmentScene()
{
}

void AssignmentScene::Init()
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)

	checkmodelStack = false;
	running = true;
	bodyMovement = true;
	b_BMO = false;
	b_viewStats = false;
	speed = 0;

	//<collison class>
	collide = false;
	rotationangle = 0;
	updatedangle = 0;
	TranslateAIX = 0;
	TranslateAIZ = 50;

	//<----for BMO body animation movement when running---->
	LeftLegX = 90.0f;
	RightLegX = 90.0f;
	ArmRotation = 0.0f;
	TranslateBodyX = 0.0f;
	//TranslateBodyY = 15.0f;
	TranslateBodyY = 0.0f;
	TranslateBodyZ = 0.0f;
	RotateBody = 0.0f;

	//<--Music-->
	b_musicSelected = false;
	b_inPM = false;
	b_inPC = false;
	
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LSPEED = 30.0f;

	camera.Init(Vector3(0, 20, 80), Vector3(0, 0, 0), Vector3(0, 1, 0));

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
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID,"material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID,"material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID,"material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID,"material.kShininess");

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

	glUseProgram(m_programID);

	light[0].type = Light::LIGHT_POINT; //light of the room
	light[0].position.Set(0.0f, 150.0f, 0.0f);//position of light
	light[0].color.Set(1, 1, 1);//color that light emits
	light[0].power = 2.0f;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	glUniform1i(m_parameters[U_NUMLIGHTS], 1);//if you add lights, add number here

	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("Light Sphere", Color(1.0f, 1.0f, 1.0f), 18, 36, 1.0f, 360.0f);
	meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere( "Sphere", Color(1.0f, 1.0f, 1.0f), 18, 36, 1.0f, 360.0f);

	//Guide lines - Turn on if need
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Reference", 1000.0f, 1000.0f, 1000.0f);

	//<----BMO body---->
	meshList[GEO_BODY] = MeshBuilder::GenerateCube("Body", Color(0.3671875f, 0.7734375f, 0.7890625f), 5.0f, 7.0f, 3.0f);
	meshList[GEO_BODY]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_BODY]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_BODY]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_BODY]->material.kShininess = 1.0f;

	meshList[GEO_BOX0] = MeshBuilder::GenerateCube("Box", Color(0, 0, 1), 3.0f, 3.0f, 3.0f);
	meshList[GEO_BOX] = MeshBuilder::GenerateCube("Box", Color(1, 0, 0), 3.0f, 3.0f, 3.0f);
	meshList[GEO_BOX2] = MeshBuilder::GenerateCube("Box", Color(1, 1, 1), 3.0f, 3.0f, 3.0f);
	//Obj[OBJ_PLAYER] = new ObjectBox(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ), 3.0f, 3.0f, 3.0f);
	Obj[OBJ_BOX0] = new ObjectBox(Vector3(TranslateAIX, 0.0f, TranslateAIZ), 6.0f, 6.0f, 6.0f);
	Obj[OBJ_BOX] = new ObjectBox(Vector3(50.0f, 0.0f, 0.0f), 6.0f, 6.0f, 6.0f);
	Obj[OBJ_BOX2] = new ObjectBox(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ), 6.0f, 6.0f, 6.0f);
	//Obj[OBJ_BOX2] = new ObjectBox(Vector3(camera.position.x, camera.position.y, camera.position.z), 10.0f, 10.0f, 10.0f);
	//<----Face---->
	meshList[GEO_FACE] = MeshBuilder::GenerateCube("Face", Color(0.7109375f, 0.99609375f, 0.77734375f), 4.0f, 2.6f, 3.0f);
	meshList[GEO_FACE]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_FACE]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_FACE]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_FACE]->material.kShininess = 1.0f;

	meshList[GEO_FACEOFF] = MeshBuilder::GenerateCube("Face", Color(0.82745098039f, 0.82745098039f, 0.82745098039f), 4.0f, 2.6f, 3.0f);
	meshList[GEO_FACEOFF]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_FACEOFF]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_FACEOFF]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_FACEOFF]->material.kShininess = 1.0f;

	//<----Leg part---->
	meshList[GEO_LEG] = MeshBuilder::GenerateCylinder("Leg", Color(0.3171875f, 0.7234375f, 0.7390625f), 360.0f, 1.0f, -3.0f);
	meshList[GEO_LEG]->material.kAmbient.Set(0.7f, 0.7f, 0.7f); //how strong the base color is
	meshList[GEO_LEG]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f); //how bright the surface is in relation to the light
	meshList[GEO_LEG]->material.kSpecular.Set(0.4f, 0.4f, 0.4f); //Metalic
	meshList[GEO_LEG]->material.kShininess = 1.0f;

	meshList[GEO_FEET] = MeshBuilder::GenerateSphere("Feet", Color(0.2171875f, 0.6234375f, 0.6390625f), 18, 36, 1.0f, 180.0f);
	meshList[GEO_FEET]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_FEET]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_FEET]->material.kSpecular.Set(1.0f, 1.0f, 1.0f);
	meshList[GEO_FEET]->material.kShininess = 1.0f;

	//<----Eye---->
	meshList[GEO_EYESCIRCLE] = MeshBuilder::GenerateCircle("Eyes circle", Color(0.0f, 0.0f, 0.0f), 360 ,1.0f);
	meshList[GEO_EYESCYLINDER] = MeshBuilder::GenerateCylinder("Eyes cylinder", Color(0.0f, 0.0f, 0.0f), 360.0f, 1.0f, -3.0f);

	//<----Mouth---->
	meshList[GEO_MOUTHSEMICIRCLE] = MeshBuilder::GenerateCircle("Mouth semi circle",Color(0.98823529411f, 0.69803921568f, 0.59215686274f), 180, 1.0f);
	meshList[GEO_MOUTHCYLINDER] = MeshBuilder::GenerateCylinder("Mouth cylinder", Color(0.7f, 0.68039215686f, 0.50392156862f), 180.0f, 1.0f, -3.0f);
	meshList[GEO_MOUTHQUAD] = MeshBuilder::GenerateQuad("Mouth quad", Color(0.7f, 0.68039215686f, 0.50392156862f), 1.0f, 0.0f, 1.0f);

	//<----Arm---->
	meshList[GEO_ARMS] = MeshBuilder::GenerateCylinder("Arms", Color(0.4671875f, 0.8734375f, 0.8890625f), 360.0f, 0.5f, -0.8f);
	meshList[GEO_ARMS]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_ARMS]->material.kDiffuse.Set(0.9f, 0.9f, 0.9f);
	meshList[GEO_ARMS]->material.kSpecular.Set(1.0f, 1.0f, 1.0f);
	meshList[GEO_ARMS]->material.kShininess = 1.0f;

	meshList[GEO_HAND] = MeshBuilder::GenerateSphere("Hand", Color(0.3671875f, 0.7734375f, 0.7890625f), 18, 36, 1.0f, 360.0f);
	meshList[GEO_HAND]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_HAND]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_HAND]->material.kSpecular.Set(1.0f, 1.0f, 1.0f);
	meshList[GEO_HAND]->material.kShininess = 1.0f;

	//<----Body "parts"---->
	//<--Yellow "+" buttons-->
	meshList[GEO_YELLOWBUTTONCUBOID] = MeshBuilder::GenerateCube("Yellow button horizontal", Color(1.0f, 1.0f, 0.0f), 0.75f, 2.0f, 5.0f);
	meshList[GEO_YELLOWBUTTONCUBOID]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_YELLOWBUTTONCUBOID]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_YELLOWBUTTONCUBOID]->material.kSpecular.Set(1.0f, 1.0f, 1.0f);
	meshList[GEO_YELLOWBUTTONCUBOID]->material.kShininess = 1.0f;

	//<--USB-->
	meshList[GEO_USB] = MeshBuilder::GenerateCube("USB", Color(0.01171875f, 0.19140625f, 0.23046875f), 2.0f, 0.4f, 3.0f);
	meshList[GEO_USB]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_USB]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_USB]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_USB]->material.kShininess = 1.0f;

	//<--Blue button circle-->
	meshList[GEO_BLUEBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Blue button circle", Color(0.0f, 0.0f, 0.74296875f), 360, 1.0f);
	meshList[GEO_BLUEBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_BLUEBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_BLUEBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_BLUEBUTTONCIRCLE]->material.kShininess = 1.0f;

	//<--Blue button cylinder-->
	meshList[GEO_BLUEBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Blue button cylinder", Color(0.0f, 0.0f, 0.4f), 360.0f, 1.0f, -3.0f);
	meshList[GEO_BLUEBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_BLUEBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_BLUEBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_BLUEBUTTONCYLINDER]->material.kShininess = 1.0f;

	//<--Green button circle-->
	meshList[GEO_GREENBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Green button circle", Color(0.0f, 0.64296875f, 0.0f), 360, 1.0f);
	meshList[GEO_GREENBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_GREENBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GREENBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_GREENBUTTONCIRCLE]->material.kShininess = 1.0f;

	//<--Green button cylinder-->
	meshList[GEO_GREENBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Green button cylinder", Color(0.0f, 0.4f, 0.0f), 360.0f, 1.0f, -3.0f);
	meshList[GEO_GREENBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_GREENBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_GREENBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_GREENBUTTONCYLINDER]->material.kShininess = 1.0f;

	//<--Red button circle-->
	meshList[GEO_REDBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Red button circle", Color(0.64296875f, 0.0f, 0.0f), 360, 1.0f);
	meshList[GEO_REDBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_REDBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_REDBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_REDBUTTONCIRCLE]->material.kShininess = 1.0f;

	//<--Red button cylinder-->
	meshList[GEO_REDBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Red button cylinder", Color(0.4f, 0.0f, 0.0f), 360.0f, 1.0f, -3.0f);
	meshList[GEO_REDBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	meshList[GEO_REDBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_REDBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	meshList[GEO_REDBUTTONCYLINDER]->material.kShininess = 1.0f;

	//<--Turquoise triangle button-->
	meshList[GEO_TRIPRISM] = MeshBuilder::GeneratePrism("Triangular button", Color(0.0f, 0.8f, 0.8f), 1.0f, 1.0f, 5.0f);
	meshList[GEO_TRIPRISM]->material.kAmbient.Set(0.80f, 0.80f, 0.80f); 
	meshList[GEO_TRIPRISM]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f); 
	meshList[GEO_TRIPRISM]->material.kSpecular.Set(0.5f, 0.5f, 0.5f); 
	meshList[GEO_TRIPRISM]->material.kShininess = 1.0f;

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[GEO_POKECWALLWW] = MeshBuilder::GenerateQuad("POKE wall", Color(1, 1, 1), 1.0f, 0.0f, 1.0f); //left, right, front,back
	meshList[GEO_POKECWALLWW]->textureID = LoadTGA("Image//PokeCenter_wallsww.tga");
	meshList[GEO_POKECWALLWW]->material.kAmbient.Set(0.50f, 0.50f, 0.50f);
	meshList[GEO_POKECWALLWW]->material.kDiffuse.Set(1.0f, 1.0f, 1.0f);
	meshList[GEO_POKECWALLWW]->material.kSpecular.Set(2.5f, 2.5f, 2.5f);
	meshList[GEO_POKECWALLWW]->material.kShininess = 1.0f;

	meshList[GEO_POKECFLOOR] = MeshBuilder::GenerateQuad("Pokecenter floor", Color(1, 1, 1), 1.0f, 0.0f, 1.0f); //left, right, front, back
	meshList[GEO_POKECFLOOR]->textureID = LoadTGA("Image//PokeCenter_floor.tga");

	meshList[GEO_POKECEILING] = MeshBuilder::GenerateQuad("Pokecenter ceiling", Color(1, 1, 1), 1.0f, 0.0f, 1.0f); //top
	meshList[GEO_POKECEILING]->textureID = LoadTGA("Image//Poke_Ceiling.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	
}

void AssignmentScene::PlayMusic()
{
	if ((TranslateBodyX > 30 && (TranslateBodyZ > 0 && TranslateBodyZ < 90)))
		b_checkinPM = true;
	else
	{
		b_checkinPM = false;
		b_inPM = false;
	}

	if (b_checkinPM && !b_inPM)
	{
		if (!b_inPM)
		{
			b_musicSelected = false;
			b_inPM = true;
		}
		b_inPC = false;
	}
	else if(!b_inPC && !b_inPM)
	{
		if (!b_inPC)
		{
			b_musicSelected = false;
			b_inPC = true;
		}
		b_musicSelected = false;
		b_inPM = false;
	}

	if (!b_musicSelected)
	{
		if (b_inPM)
		{
			PlaySound(TEXT("Audio//PokeMart.wav"), NULL, SND_ASYNC | SND_LOOP);
			b_musicSelected = true;
		}
		else
		{
			PlaySound(TEXT("Audio//PokeCenter.wav"), NULL, SND_ASYNC | SND_LOOP);
			b_musicSelected = true;
		}
	}
}

void AssignmentScene::Update(double dt)
{
	if (Application::IsKeyPressed('5'))
	{
		Application app;
		app.SetSceneNumber(1);
		app.Run();
	}
	if (Application::IsKeyPressed('6'))
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
	}
	if (Application::IsKeyPressed('0'))
	{
		TranslateBodyY = 0.0f;
		//TranslateBodyY = 15.0f;
		TranslateBodyZ = 0.0f;
	}
	if (Application::IsKeyPressed('T'))
	{	
		RotateBody = 180.0f;
		if (TranslateBodyZ > -90.0f)
		{
		TranslateBodyZ -= (float)(dt * 16);
		checkmodelStack = true;
		}
	}
	if (Application::IsKeyPressed('F'))
	{	
		RotateBody = 270.0f;
		if (TranslateBodyX > -90.0f)
		{
		TranslateBodyX -= (float)(dt * 16);
		checkmodelStack = true;
		}
	}
	if (Application::IsKeyPressed('G'))
	{	
		RotateBody = 0.0f;
		if (TranslateBodyZ < 90.0f)
		{
		TranslateBodyZ += (float)(dt * 16);
		checkmodelStack = true;
		}
	}
	if (Application::IsKeyPressed('H'))
	{	
		RotateBody = 90.0f;
		if (TranslateBodyX < 90.0f)
		{
		TranslateBodyX += (float)(dt * 16);
		checkmodelStack = true;
		}
	}
	else
	{
		checkmodelStack = false;
		running = true;
		bodyMovement = true;
		LeftLegX = 90.0f;
		RightLegX = 90.0f;
		ArmRotation = 0.0f;
		//TranslateBodyY = 15.0f;
	}

	if (Application::IsKeyPressed(VK_LEFT))
	{
		TranslateAIX -= (float)(dt * 16);
	}
	if (Application::IsKeyPressed(VK_RIGHT))
	{
		TranslateAIX += (float)(dt * 16);
	}
	if (Application::IsKeyPressed(VK_UP))
	{
		TranslateAIZ -= (float)(dt * 16);
	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		TranslateAIZ += (float)(dt * 16);
	}

	if (Application::IsKeyPressed('P')) //Pointer pointing to an object
		b_viewStats = true;
	else
		b_viewStats = false;

	if (Application::IsKeyPressed('V'))
	{
		speed += (20 * dt);
	}
	if (Application::IsKeyPressed('B'))
	{
		speed -= (20 * dt);
	}

	speed -= speed * 0.2 * (dt); 
	TranslateBodyZ += speed * dt;

	if (Application::IsKeyPressed('1') && !(ObjectBox::checkCollision(*Obj[OBJ_BOX2], *Obj[OBJ_BOX])))
	{
		rotationangle += 1.0f;
		updatedangle = 1.0f;
	}
	else if (Application::IsKeyPressed('2') && !(ObjectBox::checkCollision(*Obj[OBJ_BOX2], *Obj[OBJ_BOX])))
	{
		rotationangle -= 1.0f;
		updatedangle = -1.0f;
	}

	//for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	//{
	//	if (!ObjectBox::checkCollision(*Obj[OBJ_BOX2], *Obj[AllObjs]))
	//	{
	//		if (Application::IsKeyPressed('1'))
	//		{
	//			rotationangle += 1.0f;
	//			updatedangle = 1.0f;
	//		}
	//		else if (Application::IsKeyPressed('2'))
	//		{
	//			rotationangle -= 1.0f;
	//			updatedangle = -1.0f;
	//		}
	//	}
	//	else
	//		break;
	//}

	Obj[OBJ_BOX2]->setRotatingAxis(updatedangle, 0.0f, 1.0f, 0.0f);
	updatedangle = 0;
	Obj[OBJ_BOX2]->setOBB(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));
	Obj[OBJ_BOX0]->setOBB(Vector3(TranslateAIX, 0, TranslateAIZ));

	//<collision>
	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_BOX2], *Obj[AllObjs]))
		{
			collide = true;
			//camera.position = currentCamPos;
			//camera.target = currentCamTarget;
			TranslateBodyX = prevBodyX;
			TranslateBodyZ = prevBodyZ;
			rotationangle = prevAngle;
			break;
		}
		collide = false;
	}

	if (!collide)
	{
		//currentCamPos = camera.position;
		//currentCamTarget = camera.target;
		prevBodyX = TranslateBodyX;
		prevBodyZ = TranslateBodyZ;
		prevAngle = rotationangle;
	}

	//If collision is true, disable player movement,
	//when the player moves, check which keypress is selected and if detected, set previous position to current;

	fps = 1.0f / (float)dt;


	if (getCurrentCam)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}

	//<For switching camera position and target temporary example>
	//if (Application::IsKeyPressed('V') && b_NearShowcase == true && i_ShowcaseItem > 0 )//must be near enough, do range check
	//{
	//	getCurrentCam = false;
	//	b_BMO = false;
	//	if (i_ShowcaseItem == 1)//Cubone
	//	{
	//		camera.target = Vector3(-65.295425f, 23.506683f, 5.0808f);
	//		camera.position = Vector3(-64.389885f, 23.570976f, 4.6615f);

	//	}
	//	else if (i_ShowcaseItem == 2)//Beast ball
	//	{
	//		camera.target = Vector3(-67.434624f, 15.368983f, -4.589f);
	//		camera.position = Vector3(-66.434624f, 15.468983f, -4.889f);
	//	}
	//	else if (i_ShowcaseItem == 3)//Pokeball
	//	{
	//		camera.target = Vector3(-68.513283f, 22.983822f, -19.27f);
	//		camera.position = Vector3(-67.562569f, 23.133141f, -19.54f);
	//	}
	//}
	//else
	//{
	//	camera.position = currentCamPos;
	//	camera.target = currentCamTarget;
	//	getCurrentCam = true;
	//	b_BMO = true;
	//}

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



	//PlayMusic();
	camera.Update(dt);
}

void AssignmentScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderSkybox();
	RenderPokeCenter();

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

	modelStack.PushMatrix();
	modelStack.Translate(0, 5, 10);
	modelStack.Scale(5, 5, 5);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();


	////<-----------Axes----------->
	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix(); 
		
	modelStack.PushMatrix();
	modelStack.Translate(TranslateAIX,0, TranslateAIZ);
	RenderMesh(meshList[GEO_BOX0], false);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	modelStack.Translate(50,0,0);
	RenderMesh(meshList[GEO_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ);
	modelStack.Rotate(rotationangle, 0, 1, 0);
	RenderMesh(meshList[GEO_BOX2], false);
	modelStack.PopMatrix();

	//<--BMO-->
	if (b_BMO)
	{
		//<-----------Body----------->
		modelStack.PushMatrix();
		modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ);
		modelStack.Rotate(RotateBody, 0.0f, 1.0f, 0.0f);
		modelStack.Scale(1.5f, 1.5f, 1.5f);
		RenderMesh(meshList[GEO_BODY], false);
		

		//<-----------Right Arms----------->
		modelStack.PushMatrix();
		modelStack.Translate(5.0f, -1.0f, 0.0f);
		modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
		modelStack.Rotate(ArmRotation, 0.0f, 0.0f, 1.0f);
		modelStack.Scale(1.2f, 1.2f, 1.2f);
		RenderMesh(meshList[GEO_ARMS], false);


		for (int x = 0; x < 9; ++x)
		{
			modelStack.PushMatrix();
			modelStack.Translate(0.0f, -0.1f, 0.5f);
			modelStack.Rotate(10.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(meshList[GEO_ARMS], false);
		}

		//<-----------Right Hand----------->
		modelStack.PushMatrix();
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_HAND], false);

		for (int x = 0; x < 10; ++x)
		{
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();

		//<-----------Left Arms----------->
		modelStack.PushMatrix();
		modelStack.Translate(-5.0f, -1.0f, 0.0f);
		modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
		modelStack.Rotate(ArmRotation, 0.0f, 0.0f, 1.0f);
		modelStack.Scale(1.2f, 1.2f, 1.2f);
		RenderMesh(meshList[GEO_ARMS], false);

		for (int x = 0; x < 9; ++x)
		{
			modelStack.PushMatrix();
			modelStack.Translate(0.0f, -0.1f, 0.5f);
			modelStack.Rotate(10.0f, 1.0f, 0.0f, 0.0f);
			RenderMesh(meshList[GEO_ARMS], false);
		}

		//<-----------Left Hand----------->
		modelStack.PushMatrix();
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_HAND], false);

		for (int x = 0; x < 10; ++x)
		{
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();

		//<-----------Left Legs----------->
		modelStack.PushMatrix();
		modelStack.Rotate(LeftLegX, 1.0f, 0.0f, 0.0f);
		modelStack.Translate(-2.0f, 0.0f, 10.0f);
		modelStack.Scale(1.0f, 1.0f, 1.3f);
		RenderMesh(meshList[GEO_LEG], false);

		//<-----------Left Feet----------->
		modelStack.PushMatrix();
		modelStack.Rotate(180.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(1.0f, 1.5f, 1.0f);
		modelStack.Translate(0.0f, -0.4f, 0.0f);
		RenderMesh(meshList[GEO_FEET], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		//<-----------Right Legs----------->
		modelStack.PushMatrix();
		modelStack.Rotate(RightLegX, 1.0f, 0.0f, 0.0f);
		modelStack.Translate(2.0f, 0.0f, 10.0f);
		modelStack.Scale(1.0f, 1.0f, 1.3f);
		RenderMesh(meshList[GEO_LEG], false);

		//<-----------Right Feet----------->
		modelStack.PushMatrix();
		modelStack.Rotate(180.0f, 1.0f, 0.0f, 0.0f);
		modelStack.Scale(1.0f, 1.5f, 1.0f);
		modelStack.Translate(0.0f, -0.4f, 0.0f);
		RenderMesh(meshList[GEO_FEET], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		//<-----------Face----------->
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 3.5f, 0.03f);
		modelStack.Scale(1.0f, 1.0f, 1.0f);
		RenderMesh(meshList[GEO_FACE], false);

		//<-----------Left Eyes----------->
		modelStack.PushMatrix();
		modelStack.Translate(-2.0f, 0.6f, 3.2f);
		modelStack.Scale(0.55f, 0.55f, 0.55f);
		RenderButton(GEO_EYESCIRCLE, GEO_EYESCYLINDER);
		modelStack.PopMatrix();

		//<-----------Right Eyes----------->
		modelStack.PushMatrix();
		modelStack.Translate(2.0f, 0.6f, 3.2f);
		modelStack.Scale(0.55f, 0.55f, 0.55f);
		RenderButton(GEO_EYESCIRCLE, GEO_EYESCYLINDER);
		modelStack.PopMatrix();

		//<-----------Mouth semi circle----------->
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 0.6f, 3.2f);
		modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
		RenderMesh(meshList[GEO_MOUTHSEMICIRCLE], false);

		//<-----------Mouth semi cylinder----------->
		modelStack.PushMatrix();
		modelStack.Rotate(180.0f, 0.0f, 0.0f, 1.0f);
		RenderMesh(meshList[GEO_MOUTHCYLINDER], false);

		//<-----------Mouth quad----------->
		modelStack.PushMatrix();
		modelStack.Translate(0.0f, 0.0f, -1.0f);
		RenderMesh(meshList[GEO_MOUTHQUAD], false);

		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PopMatrix();

		//<-----------USB----------->
		modelStack.PushMatrix();
		modelStack.Translate(-2.0f, -1.0f, 0.01f);
		RenderMesh(meshList[GEO_USB], false);
		modelStack.PopMatrix();

		//<-----------Dark Blue Button----------->
		modelStack.PushMatrix();
		modelStack.Translate(3.0f, -1.0f, 3.4f);
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderButton(GEO_BLUEBUTTONCIRCLE, GEO_BLUEBUTTONCYLINDER);
		modelStack.PopMatrix();

		//<-----------Green Button----------->
		modelStack.PushMatrix();
		modelStack.Translate(3.5f, -3.5f, 3.4f);
		modelStack.Scale(0.35f, 0.35f, 0.35f);
		RenderButton(GEO_GREENBUTTONCIRCLE, GEO_GREENBUTTONCYLINDER);
		modelStack.PopMatrix();

		//<-----------Red Button----------->
		modelStack.PushMatrix();
		modelStack.Translate(2.5f, -4.5f, 3.4f);
		modelStack.Scale(0.55f, 0.55f, 0.55f);
		RenderButton(GEO_REDBUTTONCIRCLE, GEO_REDBUTTONCYLINDER);
		modelStack.PopMatrix();


		//<-----------Triangle Button----------->
		modelStack.PushMatrix();
		modelStack.Translate(2.2f, -2.6f, 3.4f);
		modelStack.Scale(0.6f, 0.6f, 0.6f);
		RenderMesh(meshList[GEO_TRIPRISM], false);

		modelStack.PopMatrix();

		//<-----------Movement "+" Button----------->
		modelStack.PushMatrix();
		modelStack.Translate(-2.0f, -3.5f, 0.7f);
		modelStack.Scale(0.55f, 0.55f, 0.55f);
		RenderMesh(meshList[GEO_YELLOWBUTTONCUBOID], false);

		modelStack.PushMatrix();
		modelStack.Rotate(90, 0.0f, 0.0f, 1.0f);
		RenderMesh(meshList[GEO_YELLOWBUTTONCUBOID], false);

		modelStack.PopMatrix();
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	}
	//<--BMO-->

	if (b_viewStats)
	{
	//<--FPS-->
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("FPS:" + std::to_string(fps)), Color(0, 0, 0), 2, 52, 58);
	modelStack.PopMatrix();

	//<--Get BMOS x position-->
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("X:" + std::to_string(TranslateBodyX)), Color(0, 0, 0), 2, 56, 56);
	modelStack.PopMatrix();

	//<--Get BMOS z position-->
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Z:" + std::to_string(TranslateBodyZ)), Color(0, 0, 0), 2, 56, 54);
	modelStack.PopMatrix();
	}
	else
	{
		//<--View stats for nerds-->
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("View stats:[P]"), Color(0, 0, 0), 2, 54, 58);
		modelStack.PopMatrix();
	}

	if (collide)
	{
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("Collide"), Color(0, 0, 0), 2, 52, 50);
		modelStack.PopMatrix();
	}
	else
	{
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("No Collide"), Color(0, 0, 0), 2, 54, 50);
		modelStack.PopMatrix();
	}

	int speedct = abs(speed);
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(speedct), Color(1, 1, 1), 3, 2, 55);
	modelStack.PopMatrix(); 

	//<--Get cameras position-->
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Pos X:" + std::to_string(camera.position.x)+", Y:"+ std::to_string(camera.position.y) +" , Z:"+ std::to_string(camera.position.z)), Color(0, 1, 0), 2, 2, 5);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Tar X:" + std::to_string(camera.target.x)+", Y:"+ std::to_string(camera.target.y) +" , Z:"+ std::to_string(camera.target.z)), Color(1, 0, 0), 2, 2, 7);
	modelStack.PopMatrix();



}

void AssignmentScene::RenderMesh(Mesh *mesh, bool enableLight)
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

void AssignmentScene::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 0.96f, -0.98f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 0.96f, 0.98f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 1.94f, 0.0f);
	modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	modelStack.Translate(0.0f, -0.02f, 0.0f);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(-0.98f, 0.96f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.98f, 0.96f, 0.0f);
	modelStack.Rotate(-90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
}

void AssignmentScene::RenderPokeCenter()
{
	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.0f, -0.98f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECWALLWW], true); //back
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.0f, 0.98f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECWALLWW], true); //front
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.5f, 0.0f);
	modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECEILING], false); //top
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECFLOOR], false); //bottom
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(-0.98f, 1.0f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECWALLWW], true); //left
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.98f, 1.0f, 0.0f);
	modelStack.Rotate(-90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_POKECWALLWW], true); //right
	modelStack.PopMatrix();
}

void AssignmentScene::RenderButton(int geo_circle, int geo_cylinder)
{
	//<----Button circle---->
	RenderMesh(meshList[geo_circle], false);

	//<----Button cylinder---->
	modelStack.PushMatrix();
	RenderMesh(meshList[geo_cylinder], false);
	modelStack.PopMatrix();
}

void AssignmentScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void AssignmentScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void AssignmentScene::Exit()
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
