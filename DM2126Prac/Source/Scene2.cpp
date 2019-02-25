#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "Scene2.h"
#include "Utility.h"
#include <string>
#include "Sound.h"

float Scene2::lastX = 0.0f;
float Scene2::lastY = 0.0f;
Camera2 Scene2::camera = Camera2();

Scene2::Scene2()
{
	first = last = forward = current = backward = NULL;
}

// Errors:
// Got slight issue when i try to use UDLR key to move to extreme top/ bot
// If i use mouse, program will crash

void Scene2::mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

Scene2::~Scene2()
{
	delete AmbulanceTimer;
}

void Scene2::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)
	AmbulanceTimer = new StopWatchTimer;
	checkmodelStack = false;
	running = true;
	bodyMovement = true;
	b_BMO = true;
	b_viewStats = false;
	speed = 0;

	delay = 0;

	// testing irrklan
	music::player.init();
	music::player.setSoundVol(0.5);
	music::player.playSound("Sound//Scene2//AmbulanceBGM.wav", true);

	//<collison class>
	collide = false;
	rotationangle = 0;
	updatedangle = 0;

	//<----for BMO body animation movement when running---->
	LeftLegX = 90.0f;
	RightLegX = 90.0f;
	ArmRotation = 0.0f;
	TranslateBodyX = 0.0f;
	TranslateBodyY = 0.0f;
	TranslateBodyZ = 0.0f;
	RotateBody = 0.0f;

	//<----for randomizing the instructions for player--->
	srand(time(NULL));
	instruction = rand() % 2;

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

	// Change here for camera initial position
	camera.Init(Vector3(0, 20, 6), Vector3(0, 0, 0), Vector3(0, 1, 0));

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

	glUseProgram(m_programID);

	light[0].type = Light::LIGHT_SPOT; //light of the room
	light[0].position.Set(0.0f, 20.0f, 0.0f);//position of light
	light[0].color.Set(1, 1, 1);//color that light emits
	light[0].power = 4.0f;
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

	//Guide lines - Turn on if need
	//meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Reference", 1000.0f, 1000.0f, 1000.0f);

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//WallWindow.tga");
	Obj[OBJ_FRONT] = new ObjectBox(Vector3(0, 15, 20), 30, 15, 0);

	// meshList[GEO_TEST1] = MeshBuilder::GenerateQuad("test", Color(0, 0, 1), 30.0f, 15.0f, 0.0f);
	// Obj[OBJ_TEST1] = new ObjectBox(Vector3(0, 15, 20), 30, 15, 0);  

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//WallWindow.tga");
	Obj[OBJ_BACK] = new ObjectBox(Vector3(0, 15, -21), 30, 15, 0);

	// meshList[GEO_TEST2] = MeshBuilder::GenerateQuad("test", Color(0, 0, 1), 30.0f, 15.0f, 0.0f);
    // Obj[OBJ_TEST2] = new ObjectBox(Vector3(0, 15, -21), 30, 15, 0); 

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//WallExit.tga");
	Obj[OBJ_LEFT] = new ObjectBox(Vector3(-30, 15, 0), 0.5, 15, 20);

	//meshList[GEO_TEST5] = MeshBuilder::GenerateCube("test", Color(0, 1, 0), 0.5, 15, 20);
	//Obj[OBJ_TEST5] = new ObjectBox(Vector3(-30, 15, 0), 0.5, 15, 20);

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//Wall.tga");
	Obj[OBJ_RIGHT] = new ObjectBox(Vector3(30, 15, 0), 0.5, 15, 20);

	//meshList[GEO_TEST6] = MeshBuilder::GenerateCube("test", Color(0, 1, 0), 0.5, 15, 20);
	//Obj[OBJ_TEST6] = new ObjectBox(Vector3(30, 15, 0), 0.5, 15, 20);

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//Wall.tga");
	Obj[OBJ_TOP] = new ObjectBox(Vector3(0, 30, 0), 30, 0, 20);

	// meshList[GEO_TEST3] = MeshBuilder::GenerateQuad("test", Color(0, 0, 1), 30, 0, 20);
	// Obj[OBJ_TEST3] = new ObjectBox(Vector3(0, 30, 0), 30, 0, 20);

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//Wall.tga");
	Obj[OBJ_BOTTOM] = new ObjectBox(Vector3(0, 0, 0), 30, 0, 20);
	
	//meshList[GEO_TEST4] = MeshBuilder::GenerateQuad("test", Color(0, 0, 1), 30, 0, 20);
	//Obj[OBJ_TEST4] = new ObjectBox(Vector3(0, 0, 0), 30, 0, 20);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	// Stuff added so far
	meshList[GEO_STRETCHER] = MeshBuilder::GenerateOBJ("Stretcher", "OBJ//Stretcher.obj");
	meshList[GEO_STRETCHER]->textureID = LoadTGA("Image//Stretcher.tga"); 
	Obj[OBJ_STRETCHER] = new ObjectBox(Vector3(0, 0, 6), 40, 24, 12);

	meshList[GEO_DEFIBRILLATOR] = MeshBuilder::GenerateOBJ("Defibrillator", "OBJ//Defibrillator2.obj");
	meshList[GEO_DEFIBRILLATOR]->textureID = LoadTGA("Image//Defibrillator2.tga");
	// Obj[OBJ_DEFIBRILLATOR] = new ObjectBox(Vector3(20, 21, 0), 2, 2, 2);

	meshList[GEO_FIRSTAIDKIT] = MeshBuilder::GenerateOBJ("FirstAidKit", "OBJ//FirstAidKit.obj");
	meshList[GEO_FIRSTAIDKIT]->textureID = LoadTGA("Image//FirstAidKit.tga");
	// Obj[OBJ_FIRSTAIDKIT] = new ObjectBox(Vector3(20, 20, -5), 3, 1, 3);

	meshList[GEO_CABINET] = MeshBuilder::GenerateOBJ("Cabinet", "OBJ//Cabinet.obj"); // main cabinet
	meshList[GEO_CABINET]->textureID = LoadTGA("Image//Cabinet.tga"); 
	 Obj[OBJ_CABINET] = new ObjectBox(Vector3(-15, 10, -15), 16, 26, 6); 

	meshList[GEO_CABINET2] = MeshBuilder::GenerateOBJ("Cabinet2", "OBJ//Cabinet2Working.obj"); // med cabinet
	meshList[GEO_CABINET2]->textureID = LoadTGA("Image//Cabinet2Working.tga"); 
	Obj[OBJ_CABINET2] = new ObjectBox(Vector3(-23, 10, 12), 14, 24, 8);

	meshList[GEO_CHAIR] = MeshBuilder::GenerateOBJ("Chair", "OBJ//Chair.obj");
	meshList[GEO_CHAIR]->textureID = LoadTGA("Image//Chair.tga");
	 Obj[OBJ_CHAIR] = new ObjectBox(Vector3(20, 10, 13), 8, 12, 7); 

	meshList[GEO_TOPSHELVE1] = MeshBuilder::GenerateOBJ("TopShelve1", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE1]->textureID = LoadTGA("Image//TopShelve1.tga");
	 Obj[OBJ_TOPSHELVE1] = new ObjectBox(Vector3(3, 23, -15), 20, 5, 5);

	meshList[GEO_TOPSHELVE2] = MeshBuilder::GenerateOBJ("TopShelve2", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE2]->textureID = LoadTGA("Image//TopShelve2.tga");
	 Obj[OBJ_TOPSHELVE2] = new ObjectBox(Vector3(-10, 23, 15), 20, 5, 5);

	meshList[GEO_TOPSHELVE3] = MeshBuilder::GenerateOBJ("TopShelve3", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE3]->textureID = LoadTGA("Image//TopShelve3.tga");
	 Obj[OBJ_TOPSHELVE3] = new ObjectBox(Vector3(10, 23, 15), 20, 5, 5);

	meshList[GEO_METALSHELVE] = MeshBuilder::GenerateOBJ("MetalShelve", "OBJ//MetalShelve2.obj");
	meshList[GEO_METALSHELVE]->textureID = LoadTGA("Image//MetalShelveNew.tga");
	Obj[OBJ_METALSHELVE] = new ObjectBox(Vector3(22.0f, 5.0f, -3.0f), 10, 30, 30); 

	// Collision Box for Camera/Player -> brot from A2 Scene
	meshList[GEO_PLAYER] = MeshBuilder::GenerateCube("Box", Color(0, 0, 1), 3.0f, 3.0f, 3.0f); // 10, 15, 15
	Obj[OBJ_PLAYER] = new ObjectBox(Vector3(camera.position.x, camera.position.y, camera.position.z), 10.0f, 10.0f, 10.0f);

	// Random stuff to test collision box of individual obj
	//meshList[GEO_TEST] = MeshBuilder::GenerateCube("Test", Color(0, 0, 1), 7.0f, 12.0f, 4.0f);
	//Obj[OBJ_TEST] = new ObjectBox(Vector3(-23, 10, 12), 14, 24, 8);



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

	// User Interface
	meshList[GEO_FRAME] = MeshBuilder::GenerateQuad("noti", Color(0, 0, 1), 25, 20, 0);
	meshList[GEO_FRAME]->textureID = LoadTGA("Image//FrameNew.tga");

	meshList[GEO_HELP] = MeshBuilder::GenerateQuad("noti", Color(0, 0, 1), 14, 2, 0);
	meshList[GEO_HELP]->textureID = LoadTGA("Image//FrameNew.tga");

	// Switching Stage
	meshList[GEO_START] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 25, 20, 0);
	meshList[GEO_START]->textureID = LoadTGA("Image//Stage2.tga");

}

//void Scene2::PlayMusic()
//{
//	if ((TranslateBodyX > 30 && (TranslateBodyZ > 0 && TranslateBodyZ < 90)))
//		b_checkinPM = true;
//	else
//	{
//		b_checkinPM = false;
//		b_inPM = false;
//	}
//
//	if (b_checkinPM && !b_inPM)
//	{
//		if (!b_inPM)
//		{
//			b_musicSelected = false;
//			b_inPM = true;
//		}
//		b_inPC = false;
//	}
//	else if (!b_inPC && !b_inPM)
//	{
//		if (!b_inPC)
//		{
//			b_musicSelected = false;
//			b_inPC = true;
//		}
//		b_musicSelected = false;
//		b_inPM = false;
//	}
//
//	if (!b_musicSelected)
//	{
//		if (b_inPM)
//		{
//			PlaySound(TEXT("Audio//PokeMart.wav"), NULL, SND_ASYNC | SND_LOOP);
//			b_musicSelected = true;
//		}
//		else
//		{
//			PlaySound(TEXT("Audio//PokeCenter.wav"), NULL, SND_ASYNC | SND_LOOP);
//			b_musicSelected = true;
//		}
//	}
//}

void Scene2::Update(double dt)
{
	delay += dt;

	score = score + 0.2;

	if (score > 20)
	{
		showIntro = false;
	}

	if (Application::IsKeyPressed('1'))
	{
		
	}
	if (Application::IsKeyPressed('2'))
	{
		printNext();
	}
	if (nextStage || Application::IsKeyPressed('3'))
	{
		// leaderboard thingy
		ofstream saveToFile("loli.txt", fstream::app);
		saveToFile << score << endl;
		
		//Application app;
		//app.SetSceneNumber(4); // go to RaceScene when done here
		//app.Run();
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
		TranslateBodyY = 15.0f;
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
	else if (Application::IsKeyPressed('F'))
	{
		RotateBody = 270.0f;
		if (TranslateBodyX > -90.0f)
		{
			TranslateBodyX -= (float)(dt * 16);
			checkmodelStack = true;
		}
	}
	else if (Application::IsKeyPressed('G'))
	{
		RotateBody = 0.0f;
		if (TranslateBodyZ < 90.0f)
		{
			TranslateBodyZ += (float)(dt * 16);
			checkmodelStack = true;
		}
	}
	else if (Application::IsKeyPressed('H'))
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
		TranslateBodyY = 15.0f;

	}

	if (Application::IsKeyPressed('P'))
		b_viewStats = true;
	else
		b_viewStats = false;

	if (camera.position.x > 10 && camera.position.x < 13)
	{
		if (Application::IsKeyPressed('C') && (delay > 0.3)) // Get key delay from sihan
		{
			delay = 0;
			collectDefi = true;
			notification1 = true;

			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene2//PickUp.wav");
			
		}
		// not suppose to be in actual game
		// cheat key for obj to reappear
		if (Application::IsKeyPressed('V') && (delay>0.3)) 
		{
			delay = 0;
			collectDefi = false;
			//if (collectDefi == false)
			//{
			//	uploadItem(8);
			//}
		}
	}

	if (camera.position.x > -3 && camera.position.x < 3 && 
		camera.position.z > -8 && camera.position.z < -5)
	{ 
		if (Application::IsKeyPressed('C'))
		{
			collectKit = true;
			notification2 = true;

			music::player.init();
			music::player.setSoundVol(0.5);
			music::player.playSound("Sound//Scene2//PickUp.wav");
		}
		if (Application::IsKeyPressed('V'))
		{
			collectKit = false;
		}
	}

	if (Application::IsKeyPressed('M')) // Defi
	{
		notification1 = false;
	}

	if (Application::IsKeyPressed('N')) // Kit
	{
		notification2 = false;
	}
	
	if (camera.position.x > 0 && camera.position.x < 5)
	{
		if (Application::IsKeyPressed('U') && collectDefi)
		{
			useDefi = true;
		}

		if (Application::IsKeyPressed('I') && collectKit)

		{
			useKit = true;
		}
	}

	// Collision Box
	Obj[OBJ_PLAYER]->setOBB(Vector3(camera.position.x, camera.position.y, camera.position.z));

	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[AllObjs]))
		{
			collide = true;
			camera.position = currentCamPos;
			camera.target = currentCamTarget;
			break;
		}
		collide = false;
	}

	if (!collide)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}

	if (collide)
	{
		//music::player.init();
		//music::player.setSoundVol(0.5);
		//music::player.playSound("Sound//Scene2//PickUp.wav");
	}

	// If collision is true, disable player movement,
	// When the player moves, check which keypress is selected and if detected
	// , set previous position to current

	fps = 1.0f / (float)dt;

	// Light movement
	if (Application::IsKeyPressed('I')) // backward
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K')) // forward
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J')) // left
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L')) // right
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('U')) // down
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O')) // up
		light[0].position.y += (float)(LSPEED * dt);

	//<--Walking animation--> removed

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

	// PlayMusic();
	camera.Update(dt);
}

void Scene2::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderSkybox();

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

	////<-----------Axes----------->
	//modelStack.PushMatrix();
	//RenderMesh(meshList[GEO_AXES], false);
	//modelStack.PopMatrix();

	//<--BMO--> removed



	//<-----------Light ball Sphere lighting 1----------->
	//modelStack.PushMatrix();
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//RenderMesh(meshList[GEO_LIGHTBALL], false);
	//modelStack.PopMatrix();

	//<-----------Collision Box-------------->

	//modelStack.PushMatrix();
	//modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ);
	//modelStack.Rotate(rotationangle, 0, 1, 0);
	//RenderMesh(meshList[GEO_PLAYER], false);
	//modelStack.PopMatrix();

	//<--Lego Model-->

	modelStack.PushMatrix();
	modelStack.Translate(-5, 12, 13);
	modelStack.Scale(3, 3, 3);
	modelStack.Rotate(270, 0, 0, 1);
	modelStack.Rotate(270, 0, 1, 0);
	RenderMesh(meshList[GEO_HAIR], setTrueFalse);
	RenderMesh(meshList[GEO_FACE], setTrueFalse);
	RenderMesh(meshList[GEO_BODY], setTrueFalse);
	RenderMesh(meshList[GEO_RARM], setTrueFalse);
	RenderMesh(meshList[GEO_LARM], setTrueFalse);
	RenderMesh(meshList[GEO_RHAND], setTrueFalse);
	RenderMesh(meshList[GEO_LHAND], setTrueFalse);
	RenderMesh(meshList[GEO_RLEG], setTrueFalse);
	RenderMesh(meshList[GEO_LLEG], setTrueFalse);
	RenderMesh(meshList[GEO_CROTCH], setTrueFalse);
	modelStack.PopMatrix();

	//<--Stuff in Ambulance Scene-->

	modelStack.PushMatrix();
	modelStack.Translate(0, 0, 6);
	modelStack.Scale(1.5, 1, 1.5);
	RenderMesh(meshList[GEO_STRETCHER], setTrueFalse);
	modelStack.PopMatrix();

	if (!collectDefi)
	{
		modelStack.PushMatrix();
		modelStack.Translate(20, 21, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_DEFIBRILLATOR], setTrueFalse);
		modelStack.PopMatrix();
	}	

	if (!collectKit)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, 7, -15);
		modelStack.Scale(2, 2, 2);
		modelStack.Rotate(90, 1, 0, 0);
		RenderMesh(meshList[GEO_FIRSTAIDKIT], setTrueFalse);
		modelStack.PopMatrix();
	}


	modelStack.PushMatrix();
	modelStack.Translate(-18, 0, -15);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_CABINET], setTrueFalse);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-23, 0, 15);
	modelStack.Scale(1.5, 1.5, 1.5);
	RenderMesh(meshList[GEO_CABINET2], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(-23, 10, 12);
	//RenderMesh(meshList[GEO_TEST], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(20, 0, 13);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Scale(1.5, 1.5, 1.5);
	RenderMesh(meshList[GEO_CHAIR], setTrueFalse);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(3, 23, -15);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_TOPSHELVE1], setTrueFalse); // same side as cabinet
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-10, 23, 15);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_TOPSHELVE2], setTrueFalse); // diff side as cabinet
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(10, 23, 15);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(2, 2, 2);
	RenderMesh(meshList[GEO_TOPSHELVE3], setTrueFalse); // diff side as cabinet
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(2, 2, 2);
	modelStack.Translate(10, 0, -3);
	RenderMesh(meshList[GEO_METALSHELVE], setTrueFalse);
	modelStack.PopMatrix();

	if (b_viewStats)
	{
		RenderMission();
	}

	else
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_HELP], Color(0, 0, 1), false, 1, 65, 56);
		RenderTextOnScreen(meshList[GEO_TEXT], ("View Task:[P]"), Color(1, 1, 0), 2, 54, 56);
		modelStack.PopMatrix();
	}

	//if (collide)
	//{
	//	modelStack.PushMatrix();
	//	RenderTextOnScreen(meshList[GEO_TEXT], ("Collide"), Color(1, 1, 0), 2, 66, 54);
	//	modelStack.PopMatrix();
	//}

	//else
	//{
	//	modelStack.PushMatrix();
	//	RenderTextOnScreen(meshList[GEO_TEXT], ("No Collide"), Color(1, 1, 0), 2, 60, 54);
	//	modelStack.PopMatrix();
	//}

	int speedct = abs(speed); // no idea what this does

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

	if (collectDefi && notification1)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("Defibrillator added to"), Color(0, 0, 1), 2, 20, 35);
		RenderTextOnScreen(meshList[GEO_TEXT], ("Inventory"), Color(0, 0, 1), 2, 33, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("[M] to close"), Color(0, 0, 1), 2, 29, 25);
		modelStack.PopMatrix();
	}

	if (collectKit && notification2)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("First Aid Kit added to"), Color(0, 0, 1), 2, 20, 35);
		RenderTextOnScreen(meshList[GEO_TEXT], ("Inventory"), Color(0, 0, 1), 2, 33, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("[M] to close"), Color(0, 0, 1), 2, 29, 25);
		modelStack.PopMatrix();
	}

	if (showIntro)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_START], Color(0, 0, 1), false, 1, 40, 30);
		modelStack.PopMatrix();
	}

	EndMission();

}

void Scene2::RenderMission() // has transparent box now
{
	if (instruction == 0)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_FRAME], Color(1, 1, 0), false, 1, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("1. Find Defibrillator"), Color(0, 0, 1), 2, 19, 27);
		RenderTextOnScreen(meshList[GEO_TEXT], ("2. Collect it [C]"), Color(0, 0, 1), 2, 19, 23);
		RenderTextOnScreen(meshList[GEO_TEXT], ("3. Use it on Patient[U]"), Color(0, 0, 1), 2, 19, 18);
		modelStack.PopMatrix();
	}
	else if (instruction == 1)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_FRAME], Color(1, 1, 0), false, 1, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], ("1. Find First Aid Kit"), Color(0, 0, 1), 2, 19, 28);
		RenderTextOnScreen(meshList[GEO_TEXT], ("2. Collect it [C]"), Color(0, 0, 1), 2, 19, 23);
		RenderTextOnScreen(meshList[GEO_TEXT], ("3. Use it on Patient[I]"), Color(0, 0, 1), 2, 19, 18);
		modelStack.PopMatrix();
	}
}

void Scene2::EndMission()
{
	if (instruction == 0) // use defi
	{
		if (useDefi) // which is correct
		{
			modelStack.PushMatrix();
			DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], ("MISSION"), Color(0, 0, 1), 2, 34, 32);
			RenderTextOnScreen(meshList[GEO_TEXT], ("SUCCESS"), Color(0, 0, 1), 2, 34, 28);
			modelStack.PopMatrix();
			nextStage = true;
		} 

		if (useKit) // which is wrong
		{
			modelStack.PushMatrix();
			DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], ("MISSION FAIL"), Color(0, 0, 1), 2, 30, 30);
			modelStack.PopMatrix();
			nextStage = true;
		}
		
	}

	if (instruction == 1) // use kit
	{
		if (useKit) // which is correct
		{
			modelStack.PushMatrix();
			DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], ("MISSION"), Color(0, 0, 1), 2, 34, 32);
			RenderTextOnScreen(meshList[GEO_TEXT], ("SUCCESS"), Color(0, 0, 1), 2, 34, 28);
			modelStack.PopMatrix();
			nextStage = true;
		}

		if (useDefi) // which is wrong
		{
			modelStack.PushMatrix();
			DrawHUD(meshList[GEO_FRAME], Color(0, 0, 1), false, 1, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], ("MISSION FAIL"), Color(0, 0, 1), 2, 30, 30);
			modelStack.PopMatrix();
			nextStage = true;
		}
	}
}

void Scene2::RenderMesh(Mesh *mesh, bool enableLight)
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

static const float SKYBOXSIZE = 30.f;

void Scene2::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, 15.0f, 20.0f);
	modelStack.Translate(0.0f, 1.0f, -1.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_BACK], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 15.0f, -21.0f);
	//RenderMesh(meshList[GEO_TEST2], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, 15.0f, 20.0f);
	modelStack.Translate(0.0f, 1.0f, 1.0f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_FRONT], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 15.0f, 20.0f);
	//RenderMesh(meshList[GEO_TEST1], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, 20.0f);
	modelStack.Translate(0.0f, 1.0f, 0.0f); // y ori 2
	modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	RenderMesh(meshList[GEO_TOP], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 30.0f, 0.0f); 
	//RenderMesh(meshList[GEO_TEST3], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, 20.0f);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	modelStack.Translate(0.0f, -0.00f, 0.0f);
	RenderMesh(meshList[GEO_BOTTOM], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(0.0f, 0.0f, 0.0f); 
	//RenderMesh(meshList[GEO_TEST4], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, 15.0f, 20.0f);
	modelStack.Translate(-1.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_LEFT], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(-30.0f, 15.0f, 0.0f);
	//RenderMesh(meshList[GEO_TEST5], setTrueFalse);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, 15.0f, 20.0f);
	modelStack.Translate(0.98f, 0.96f, 0.0f);
	modelStack.Rotate(-90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_RIGHT], setTrueFalse);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(30.0f, 15.0f, 0.0f);
	//RenderMesh(meshList[GEO_TEST6], setTrueFalse);
	//modelStack.PopMatrix();
}

void Scene2::RenderText(Mesh* mesh, std::string text, Color color)
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

void Scene2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void Scene2::DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y)
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

void Scene2::uploadItem(int newobject)
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
}

void Scene2::printNext()
{
	Item *check;
	check = current->next;
	if (check != NULL)
	{
		current = check;
		rendertag();
	}
	else if (check == NULL && current == first)
	{
		current = last;
		rendertag();
	}
	else if (check == NULL && current == last)
	{
		current = first;
		rendertag();
	}
}

void Scene2::printPrev()
{
	Item *check;
	check = current->prev;
	if (check != NULL)
	{
		current = check;
		rendertag();
	}
	else if (check == NULL && current == first)
	{
		current = last;
		rendertag();
	}
	else if (check == NULL && current == last)
	{
		current = first;
		rendertag();
	}
}

void Scene2::rendertag()
{
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

void Scene2::Exit()
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
