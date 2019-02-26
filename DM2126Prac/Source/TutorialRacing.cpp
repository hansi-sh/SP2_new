#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera2.h"
#include "GLFW/glfw3.h"
#include "TutorialRacing.h"
#include "Utility.h"
#include <string>
#include "Sound.h"

float TutorialRaceScene::lastX = 0.0f;
float TutorialRaceScene::lastY = 0.0f;
//Camera2 RaceScene::camera = Camera2();

TutorialRaceScene::TutorialRaceScene()
{
}

TutorialRaceScene::~TutorialRaceScene()
{
}

void TutorialRaceScene::Init() //defines what shader to use
{
	
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)

	b_Switch = false;
	d_BounceTime = 0.25f;
	f_TPCRotateBy = 0.0f;

	//<collison class>
	collide = false;
	AIcollide = false;
	collider1 = 0;
	collider2 = 0;

	//<----For player car---->
	TranslateBodyX = 0.0f;
	TranslateBodyY = 64.0f;
	TranslateBodyZ = -650.0f;
	RotateBody = 0.0f;

	PlayerCar.v_SetPos(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));

	V_UpdatedPlayerPos = Vector3(0, 0, 0);
	b_StepAccelerator = false;
	b_StepBrakes = false;
	b_Steer = false;
	f_RotateAmt = 0.0f;
	f_UpdatedAngle = 0.0f;

	i_CollidedWith = 0;
	i_CollidedWith2 = 0;

	f_HeightAIP = 3.25f + 3.0f;	//Player , AI

	for (int i = 0; i < 14; i++)
	{
		AIWalkX[i] = 30;
		AIWalkY[i] = 64;
		AIWalkZ[i] = ((rand() % 2750) - 1300);
		checkmove[i] = false;
		movechoice[i] = 0;
		AIpos[i] = (Vector3(AIWalkX[i], AIWalkY[i], AIWalkZ[i]));
	}
	for (int i = 15; i < 30; i++)
	{
		AIWalkX[i] = -30;
		AIWalkY[i] = 64;
		AIWalkZ[i] = ((rand() % 2750) - 1300);
		checkmove[i] = false;
		movechoice[i] = 0;
		AIpos[i] = (Vector3(AIWalkX[i], AIWalkY[i], AIWalkZ[i]));
	}
	
	//AIWALK
	bool dead = false;
	bool collideAI = true;
	for (int i = 0; i < 7; i++)
	{
		enemyX[i] = 15;
		enemyY[i] = 64;
		enemyZ[i] = ((rand() % 2750) - 1300);
		RotateEnemyBody[i] = 0.0f;
		randcheck[i] = false;
		e[i].SetEnemyPosition(Vector3(enemyX[i], enemyY[i], enemyZ[i]));
		enemyUpdatePos[i] = Vector3(0, 0, 0);
		b_ENEMYSteer[i] = false;
		f_ENEMYRotateAmt[i] = 0.0f;
	}
	for (int i = 8; i < 15; i++)
	{
		enemyX[i] = -15;
		enemyY[i] = 64;
		enemyZ[i] = ((rand() % 2750) - 1300);
		RotateEnemyBody[i] = 0.0f;
		randcheck[i] = false;
		e[i].SetEnemyPosition(Vector3(enemyX[i], enemyY[i], enemyZ[i]));
		enemyUpdatePos[i] = Vector3(0, 0, 0);
		b_ENEMYSteer[i] = false;
		f_ENEMYRotateAmt[i] = 0.0f;
	}

	f_TPCRotateBy = 0.0f;



	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LSPEED = 30.0f;

	camera.Init(Vector3(0, 100, -180), Vector3(0, 0, 0), Vector3(0, 1, 0));

	currentCamPos = camera.position;
	currentCamTarget = camera.target;
	getCurrentCam = true;

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

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

	light[0].type = Light::LIGHT_POINT; //light of the room
	light[0].position.Set(0.0f, 140.0f, 0.0f);//position of light
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
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Reference", 1000.0f, 1000.0f, 1000.0f);
	//meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 0, 0), 4.5, 7, 6.5);

	meshList[GEO_CAR] = MeshBuilder::GenerateOBJ("Car", "OBJ//enemyredcar.obj");
	meshList[GEO_AICUBE] = MeshBuilder::GenerateCube("cube", Color(0, 0, 1), 4.5, 7, 6);
	for (int i = 3; i < NUM_OBJ; i++)
	{
		Obj[i] = new ObjectBox(Vector3(0.0f, 0.0f, 0.0f), 9, 14, 12);
	}
	meshList[GEO_Pedestrains1] = MeshBuilder::GenerateOBJ("Ambulance", "OBJ//Pedestrains2.obj");
	meshList[GEO_Pedestrains1]->textureID = LoadTGA("Image//powerful_kinoko.tga");

	meshList[GEO_Pedestrains2] = MeshBuilder::GenerateOBJ("Ambulance", "OBJ//Pedestrains1.obj");
	meshList[GEO_Pedestrains2]->textureID = LoadTGA("Image//betamush.tga");
	meshList[GEO_Pedestrains3] = MeshBuilder::GenerateOBJ("Ambulance", "OBJ//Pedestrains1.obj");
	meshList[GEO_Pedestrains3]->textureID = LoadTGA("Image//oneup.tga");
	meshList[GEO_AMBULANCE] = MeshBuilder::GenerateOBJ("Ambulance", "OBJ//ambulance.obj");
	meshList[GEO_AMBULANCE]->textureID = LoadTGA("Image//ambulance.tga");
	Obj[OBJ_PLAYER] = new ObjectBox(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ), 9, 14, 12);//For Player

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front3.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back3.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right3.tga");

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left3.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top3.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom3.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_HOSPITAL] = MeshBuilder::GenerateOBJ("hospital", "OBJ//building.obj");
	meshList[GEO_HOSPITAL]->textureID = LoadTGA("Image//hospital.tga");

	meshList[GEO_RACETRACK] = MeshBuilder::GenerateOBJ("racetrack", "OBJ//racetrack.obj");
	meshList[GEO_RACETRACK]->textureID = LoadTGA("Image//racetrack.tga");

	// Switching Stage
	meshList[GEO_START] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 22, 22, 0);
	meshList[GEO_START]->textureID = LoadTGA("Image//T3P1.tga");

	meshList[GEO_START2] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 22, 22, 0);
	meshList[GEO_START2]->textureID = LoadTGA("Image//T3P2.tga");

	/*meshList[GEO_BOX1] = MeshBuilder::GenerateCube("Blue Box", Color(0, 0, 1), 10.0f, 20.0f, 1415.0f);*/
	Obj[OBJ_BOX1] = new ObjectBox(Vector3(52.0f, /*636.0f*/77, 20.0f), 20.0f, 40.0f, 2830.0f);
	/*meshList[GEO_BOX2] = MeshBuilder::GenerateCube("Red Box", Color(1, 0, 0), 10.0f, 25.0f, 568.0f);*/
	Obj[OBJ_BOX2] = new ObjectBox(Vector3(-52.0f, /*639.0f*/77, 20.0f), 20.0f, 50.0f, 2830.0f);
	if (Application::timerh == 0)
	{
		RaceTimer.v_SetRaceSceneTime(40);
	}
	else
	{
		RaceTimer.v_SetRaceSceneTime(Application::timerh);
	}
}

void TutorialRaceScene::Update(double dt)
{
	f_TPCRotateBy += 0.5f;

	d_BounceTime -= dt;

	if (Application::IsKeyPressed(VK_LEFT) && d_BounceTime < 0.0f)
	{
		if (b_Switch)
			b_Switch = false;
		else
		{
			Application app;
			app.SetSceneNumber(5);
			app.Run();
		}

		d_BounceTime = 0.25f;
	}
	else if (Application::IsKeyPressed(VK_RIGHT) && d_BounceTime < 0.0f)
	{
		if (!b_Switch)
			b_Switch = true;
		else
		{
			Application app;
			app.SetSceneNumber(4);
			app.Run();
		}
		d_BounceTime = 0.25f;
	}

	if (Application::IsKeyPressed(VK_ESCAPE))
	{
		Application app;
		app.SetSceneNumber(0);
		app.Run();
	}

	camera.Update(f_TPCRotateBy, TranslateBodyX, TranslateBodyY, TranslateBodyZ);
	f_TPCRotateBy = 0.0f;

	if (RaceTimer.d_GetRaceSceneTime() <= 0)
	{
		timerunout = true;
	}
	if (timerunout == false)
	{
		RaceTimer.v_UpdateTime(dt);
	}
	for (int i = 0; i < 10; i++)	//golden
	{
		AIwalker[i].setpos(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		AIpos[i] = AIwalker[i].walking(AIWalkX[i], AIWalkY[i], AIWalkZ[i], dt, checkmove[i], movechoice[i], 5);
		AIWalkX[i] = AIpos[i].x;
		AIWalkY[i] = AIpos[i].y;
		AIWalkZ[i] = AIpos[i].z;
	}
	for (int i = 11; i < 20; i++)	//red
	{
		AIwalker[i].setpos(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		AIpos[i] = AIwalker[i].walking(AIWalkX[i], AIWalkY[i], AIWalkZ[i], dt, checkmove[i], movechoice[i], 15);
		AIWalkX[i] = AIpos[i].x;
		AIWalkY[i] = AIpos[i].y;
		AIWalkZ[i] = AIpos[i].z;
	}
	for (int i = 21; i < 30; i++)	//green
	{
		AIwalker[i].setpos(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		AIpos[i] = AIwalker[i].walking(AIWalkX[i], AIWalkY[i], AIWalkZ[i], dt, checkmove[i], movechoice[i], 20);
		AIWalkX[i] = AIpos[i].x;
		AIWalkY[i] = AIpos[i].y;
		AIWalkZ[i] = AIpos[i].z;
	}
	
	if (Application::IsKeyPressed('1'))
	{
	}
	if (Application::IsKeyPressed('2'))
	{
	}
	if (Application::IsKeyPressed('3'))
	{
		
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

	b_StepAccelerator = false;
	b_StepBrakes = false;

	///////////////////////rotation
	if (!collide)
	{
		PlayerCar.v_UpdateCarDirection(RotateBody);
	}
	PlayerCar.v_UpdateCarSpeed(b_StepAccelerator, b_StepBrakes, b_Steer, dt);
	V_UpdatedPlayerPos = PlayerCar.V_UpdateCarPos(dt);

	TranslateBodyX = V_UpdatedPlayerPos.x;
	TranslateBodyY = V_UpdatedPlayerPos.y;
	TranslateBodyZ = V_UpdatedPlayerPos.z;

	for (int i = 0; i < 15; i++)
	{
		randomMove[i] = e[i].randchecker(randcheck[i], randomMove[i]);
		enemyUpdatePos[i] = e[i].enemyMove(V_UpdatedPlayerPos, b_StepENEMYAccelerator, b_StepENEMYBrakes, b_ENEMYSteer, dt, RotateEnemyBody[i], randomMove[i], randcheck[i]);
		RotateEnemyBody[i] = e[i].getenemyrotate();
		for (int i = 0; i < 6; i++)
		{
			if (enemyUpdatePos[i].z > 1400)
			{
				e[i].SetEnemyPosition(Vector3(15, 64, -1300));
				enemyX[i] = enemyUpdatePos[i].x;
				enemyY[i] = enemyUpdatePos[i].y;
				enemyZ[i] = enemyUpdatePos[i].z;
			}
		}
		for (int i = 7; i < 15; i++)
		{
			if (enemyUpdatePos[i].z > 1400)
			{
				e[i].SetEnemyPosition(Vector3(-15, 64, -1300));
				enemyX[i] = enemyUpdatePos[i].x;
				enemyY[i] = enemyUpdatePos[i].y;
				enemyZ[i] = enemyUpdatePos[i].z;
			}
		}
		enemyX[i] = enemyUpdatePos[i].x;
		enemyY[i] = enemyUpdatePos[i].y;
		enemyZ[i] = enemyUpdatePos[i].z;
	}

	Obj[OBJ_PLAYER]->setRotatingAxis(f_UpdatedAngle, 0.0f, 1.0f, 0.0f);
	Obj[OBJ_PLAYER]->setOBB(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));

	for (int i = 0; i < 30; i++)
	{
		Obj[i + 3]->setOBB(Vector3(AIWalkX[i], AIWalkY[i], AIWalkZ[i]));
	}

	for (int i = 0; i < 15; i++)
	{
		Obj[i + 33]->setOBB(Vector3(enemyX[i], enemyY[i], enemyZ[i]));
	}

	//<collision>
	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[AllObjs]))
		{
			if (AllObjs<=32)
			{
				collideAI = true;
				i_CollidedWith2 = AllObjs;
			}
			collide = true;
			i_CollidedWith = AllObjs;
			break;
		}
		collide = false;
		collideAI = false;
	}
	//ISSUE:AI MOVING BACK,IF HIT AI WILL FLY ,SOLUTION:FIX AI DECELERATION 
	for (int AllObjs2 = 33; AllObjs2 < NUM_OBJ; ++AllObjs2)
	{
		for (int i = 33; i < NUM_OBJ; ++i)
		{
			if (AllObjs2 == i)
				break;
			else if (ObjectBox::checkCollision(*Obj[AllObjs2], *Obj[i]))
			{
				AIcollide = true;
				collider1 = AllObjs2;
				collider2 = i;

				if (AIcollide == true)
				{
					if (enemyZ[collider1 - 33] > enemyZ[collider2 - 33])
					{

						if (enemyZ[collider1 - 33] - enemyZ[collider2 - 33] >= 6)
						{
							e[collider1 - 33].v_SetEnemySpeed(fabs((e[collider1 - 33].f_GetEnemySpeed()*1.1)));
							e[collider2 - 33].v_SetEnemySpeed(-(fabs(e[collider2 - 33].f_GetEnemySpeed()*1.0)));
						}
						else
						{
							e[collider1 - 33].v_SetEnemySpeed(-(fabs(e[collider1 - 33].f_GetEnemySpeed()*1.0)));
							e[collider2 - 33].v_SetEnemySpeed(-(fabs(e[collider2 - 33].f_GetEnemySpeed()*1.0)));
						}
					}
					else
					{
						if (enemyZ[collider2 - 33] -enemyZ[collider1 - 33] >= 6)
						{
							e[collider1 - 33].v_SetEnemySpeed(-(fabs(e[collider1 - 33].f_GetEnemySpeed()*1.0)));
							e[collider2 - 33].v_SetEnemySpeed((fabs(e[collider2 - 33].f_GetEnemySpeed()*1.1)));
						}
						else
						{
							e[collider1 - 33].v_SetEnemySpeed(-(fabs(e[collider1 - 33].f_GetEnemySpeed()*1.0)));
							e[collider2 - 33].v_SetEnemySpeed(-(fabs(e[collider2 - 33].f_GetEnemySpeed()*1.0)));
						}
					}

					Obj[collider1]->setOBB(Vector3(enemyX[collider1 - 33], enemyY[collider1 - 33], enemyZ[collider1 - 33]));
					e[collider1 - 33].SetEnemyPosition(Vector3(enemyX[collider1 - 33], enemyY[collider1 - 33], enemyZ[collider1 - 33]));
					AIcollide = false;

				}
				break;
			}
			AIcollide = false;
		}
	}
	if (collideAI)
	{
		if (i_CollidedWith2 >= 3 && i_CollidedWith2 <= 32)
		{
			if (i_CollidedWith2 >= 3 && i_CollidedWith2 <= 12)//gold
			{
				dead = true;
				PlayerCar.v_SetSpeed((fabs(PlayerCar.f_GetSpeed()) * 0.5));
				
			}
			else//rest
			{
				dead = true;
				PlayerCar.v_SetSpeed((fabs(PlayerCar.f_GetSpeed()) *0.75));
				std::cout << PlayerCar.f_GetSpeed() << std::endl;
				
			}

		}
	}
	if (collide)	//if it collides, what ever that was changed will be set to the previous frame
	{
		//PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));

		//if i_CollidedWith the numbers of Car AI
		//PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));
		//e[i_CollidedWith-1].v_SetEnemySpeed(-(e[i_CollidedWith-1].f_GetEnemySpeed() * 0.5));
		if (i_CollidedWith >= 33 && i_CollidedWith <= 48) //i_CollidedWith <= last car AI //num in object type
		{
			if (TranslateBodyZ > enemyZ[i_CollidedWith - 33])
			{
				if ((TranslateBodyZ - enemyZ[i_CollidedWith - 33]) >= f_HeightAIP)	//if AI directly hits back of the car of player
				{
					PlayerCar.v_SetSpeed((fabs(PlayerCar.f_GetSpeed()) * 1.5));
					e[i_CollidedWith - 33].v_SetEnemySpeed(-(fabs(e[i_CollidedWith - 33].f_GetEnemySpeed() * 1.0)));
				}
				else
				{
					PlayerCar.v_SetSpeed(-(fabs(PlayerCar.f_GetSpeed() * 1.0)));
					e[i_CollidedWith - 33].v_SetEnemySpeed(-(fabs(e[i_CollidedWith - 33].f_GetEnemySpeed() * 1.0)));
				}
			}
			else
			{
				if ((enemyZ[i_CollidedWith - 33] - TranslateBodyZ) >= f_HeightAIP)	//if player directly hits back of the car of AI
				{
					PlayerCar.v_SetSpeed(-(fabs(PlayerCar.f_GetSpeed() * 1.0)));
					e[i_CollidedWith - 33].v_SetEnemySpeed((fabs(e[i_CollidedWith - 33].f_GetEnemySpeed() * 1.5)));
				}
				else
				{
					PlayerCar.v_SetSpeed(-(fabs(PlayerCar.f_GetSpeed() * 1.0)));
					e[i_CollidedWith - 33].v_SetEnemySpeed(-(fabs(e[i_CollidedWith - 33].f_GetEnemySpeed() * 1.0)));
				}
			}
		}
		else if(i_CollidedWith >= 1 && i_CollidedWith <= 2)
		{
			PlayerCar.v_SetSpeed(-(fabs(PlayerCar.f_GetSpeed() * 0.5)));
		}

		/*TranslateBodyX = prevBodyX;
		TranslateBodyZ = prevBodyZ;*/
		RotateBody = prevAngle;

		Obj[OBJ_PLAYER]->setRotatingAxis((-1 * f_UpdatedAngle), 0.0f, 1.0f, 0.0f);
		Obj[OBJ_PLAYER]->setOBB(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));
		PlayerCar.v_SetPos(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));
	}
	else	//if it does not collides, what ever happened in the previous frame will be saved
	{
		/*prevBodyX = TranslateBodyX;
		prevBodyZ = TranslateBodyZ;*/
		prevAngle = RotateBody;
	}

	f_UpdatedAngle = 0.0f;
	if (dead == true)
	{
		if ((AIpos[i_CollidedWith2 - 3].z - TranslateBodyZ) >= 6)	//if player directly hits back of the car of AI
		{
			AIpos[i_CollidedWith2 - 3].z += float(100 * dt);
			AIpos[i_CollidedWith2 - 3].y += float(100 * dt);
			AIWalkY[i_CollidedWith2 - 3] = AIpos[i_CollidedWith2 - 3].y;
			AIWalkZ[i_CollidedWith2 - 3] = AIpos[i_CollidedWith2 - 3].z;
		}
		else
		{
			AIpos[i_CollidedWith2 - 3].y += float(100 * dt);
			AIWalkY[i_CollidedWith2 - 3] = AIpos[i_CollidedWith2 - 3].y;
		}
	}
	if (getCurrentCam)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}

	if (TranslateBodyZ>=1400)
	{
		//music::player.stopSound(); // end all music at the des of scene
		
		Application app;
		app.SetSceneNumber(9);
		app.Run();
	}
}

void TutorialRaceScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderSkybox();

	modelStack.PushMatrix();
	modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ - 4);
	modelStack.Translate(0, 0, 4);
	modelStack.Rotate(RotateBody, 0.0f, 1.0f, 0.0f);
	modelStack.Translate(0, 0, -4);
	RenderMesh(meshList[GEO_AMBULANCE], false);
	modelStack.PopMatrix();

	for (int i = 0; i <= 10; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		modelStack.Scale(3.5, 3.5, 3.5);
		RenderMesh(meshList[GEO_Pedestrains1], false);
		modelStack.PopMatrix();
	}
	for (int i = 11; i <= 20; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_Pedestrains2], false);
		modelStack.PopMatrix();
	}
	for (int i = 21; i <= 30; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(AIWalkX[i], AIWalkY[i], AIWalkZ[i]);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_Pedestrains3], false);
		modelStack.PopMatrix();
	}

	for (int i = 0; i <= 15; i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(enemyX[i], enemyY[i], enemyZ[i]);
		modelStack.Rotate(RotateEnemyBody[i], 0, 1, 0);
		RenderMesh(meshList[GEO_CAR], false);
		modelStack.PopMatrix();
	}

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
	modelStack.Scale(8, 8, 8);
	modelStack.Translate(0, 8, 180);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[GEO_HOSPITAL], false);
	modelStack.PopMatrix();

	for (int i = 0; i < 25; i++)
	{
		modelStack.PushMatrix();
		modelStack.Scale(8, 8, 8);
		modelStack.Translate(0, 8, 170.5f - (i*14));
		modelStack.Rotate(270, 0, 1, 0);
		RenderMesh(meshList[GEO_RACETRACK], false);
		modelStack.PopMatrix();
	}

	if (!b_Switch)
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_START], Color(0, 0, 1), false, 1, 40, 30);
		modelStack.PopMatrix();
	}
	else
	{
		modelStack.PushMatrix();
		DrawHUD(meshList[GEO_START2], Color(0, 0, 1), false, 1, 40, 30);
		modelStack.PopMatrix();
	}
	
}

void TutorialRaceScene::RenderMesh(Mesh *mesh, bool enableLight)
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

static const float SKYBOXSIZE = 1500.f;

void TutorialRaceScene::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 0.96f, -0.99f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 0.96f, 0.99f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 1.95f, 0.0f);
	modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
	/*modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);*/
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	modelStack.Translate(0.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(-0.99f, 0.96f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.99f, 0.96f, 0.0f);
	modelStack.Rotate(-90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
}

void TutorialRaceScene::RenderButton(int geo_circle, int geo_cylinder)
{
	//<----Button circle---->
	RenderMesh(meshList[geo_circle], false);

	//<----Button cylinder---->
	modelStack.PushMatrix();
	RenderMesh(meshList[geo_cylinder], false);
	modelStack.PopMatrix();
}

void TutorialRaceScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void TutorialRaceScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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


void TutorialRaceScene::DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y)
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

void TutorialRaceScene::Exit()
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
