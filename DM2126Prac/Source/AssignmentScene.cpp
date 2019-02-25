#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera2.h"
#include "GLFW/glfw3.h"
#include "AssignmentScene.h"
#include "Utility.h"
#include <string>

float AssignmentScene::lastX = 0.0f;
float AssignmentScene::lastY = 0.0f;
//Camera2 AssignmentScene::camera = Camera2();

AssignmentScene::AssignmentScene()
{

}

//void AssignmentScene::mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	float xoffset = (float)xpos - lastX;
//	float yoffset = (float)ypos - lastY;
//	float sensitivity = 0.05f;
//
//	lastX = (float)xpos;
//	lastY = (float)ypos;
//
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	Vector3 view = camera.target - camera.position;
//	Mtx44 rotate;
//	rotate.SetToRotation(-xoffset, 0.0f, 1.0f, 0.0f);
//	view = rotate * view;
//
//	Vector3 rightVector = view.Cross(camera.up);
//	rotate.SetToRotation(-yoffset, rightVector.x, rightVector.y, rightVector.z);
//	view = rotate * view;
//
//	camera.target = camera.position + view;
//}

AssignmentScene::~AssignmentScene()
{
}

void AssignmentScene::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)

	//<collison class>
	collide = false;
	rotationangle = 0;
	updatedangle = 0;

	//<----For player car---->
	TranslateBodyX = 0.0f;
	TranslateBodyY = 64.0f;
	TranslateBodyZ = -200.0f;
	RotateBody = 0.0f;

	PlayerCar.v_SetPos(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));

	V_UpdatedPlayerPos = Vector3(0, 0, 0);
	b_StepAccelerator = false;
	b_StepBrakes = false;
	b_Steer = false;
	f_RotateAmt = 0.0f;
	f_UpdatedAngle = 0.0f;

	i_CollidedWith = 0;

	f_HeightAIP = 3.25f + 3.0f;	//Player , AI

	enemyX[0] = 15;
	enemyY[0] = 64;
	enemyZ[0]= -200;
	enemyX[1] = -15;
	enemyY[1] = 64;
	enemyZ[1] = -200;
	//RotateEnemyBody = 0.0f;

	e[0].SetEnemyPosition(Vector3(enemyX[0], enemyY[0], enemyZ[0]));
	e[1].SetEnemyPosition(Vector3(enemyX[1], enemyY[1], enemyZ[1]));
	enemyUpdatePos[0] = Vector3(0, 0, 0);
	enemyUpdatePos[1] = Vector3(0, 0, 0);
	//f_RotateENEMYPrevFrame = 0.0f;
	b_StepENEMYAccelerator = true;
	b_StepENEMYBrakes = false;
	//b_ENEMYSteer = false;
	//f_ENEMYRotateAmt = 0.0f;

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
	Obj[OBJ_ENEMY1] = new ObjectBox(Vector3(0.0f, 0.0f, 0.0f), 9, 14, 12);


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
}

void AssignmentScene::Update(double dt)
{
	if (Application::IsKeyPressed('1'))
	{
		Application app;
		app.SetSceneNumber(1);
		app.Run();
	}
	if (Application::IsKeyPressed('2'))
	{
		Application app;
		app.SetSceneNumber(2);
		app.Run();
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

	if (Application::IsKeyPressed('P'))
		b_viewStats = true;
	else
		b_viewStats = false;

	// f_RotateENEMYPrevFrame = RotateEnemyBody;
	fps = 1.0f / (float)dt;

	//if (Application::IsKeyPressed('I'))//forward
	//{
	//	TranslateBodyZ += 0.3f;
	//}
	//else if (Application::IsKeyPressed('K'))//backward
	//{
	//	TranslateBodyZ -= 0.3f;
	//}
	//if (Application::IsKeyPressed('J'))//left
	//{
	//	TranslateBodyX += 0.3f;
	//}
	//else if (Application::IsKeyPressed('L'))//right
	//{
	//	TranslateBodyX -= 0.3f;
	//}
	/////////////////////////////rotation

	//f_RotateAmt = 1.0f;
	//if (Application::IsKeyPressed('Z') && !(ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[OBJ_ENEMY1])))//rotate left
	//{
	//	RotateBody += f_RotateAmt;
	//	f_UpdatedAngle = 1.0f;
	//}
	//else if (Application::IsKeyPressed('X') && !(ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[OBJ_ENEMY1])))//rotate right
	//{
	//	RotateBody -= f_RotateAmt;
	//	f_UpdatedAngle = -1.0f;
	//}


	if (Application::IsKeyPressed('W'))//forward
	{
		b_StepAccelerator = true;
		b_StepBrakes = false;
	}
	else if (Application::IsKeyPressed('S'))//backward
	{
		b_StepAccelerator = false;
		b_StepBrakes = true;
	}
	else
	{
		b_StepAccelerator = false;
		b_StepBrakes = false;
	}
	///////////////////////rotation
	if(!collide)
		{
			if (fabs(PlayerCar.f_GetSpeed()) < 3.0f)
			{
				f_RotateAmt = 0.0f;
			}
			else if (fabs(PlayerCar.f_GetSpeed()) < 20.0f)
			{
				f_RotateAmt = 0.3f;
			}
			else if (fabs(PlayerCar.f_GetSpeed()) < 40.0f)
			{
				f_RotateAmt = 0.5f;
			}
			else if (fabs(PlayerCar.f_GetSpeed()) < 60.0f)
			{
				f_RotateAmt = 1.0f;
			}

			if (Application::IsKeyPressed('A'))//rotate left
			{
				b_Steer = true;
				f_UpdatedAngle = (RotateBody + f_RotateAmt) - RotateBody;
				RotateBody += f_RotateAmt;
			}
			else if (Application::IsKeyPressed('D'))//rotate left
			{
				b_Steer = true;
				f_UpdatedAngle = (RotateBody - f_RotateAmt) - RotateBody;
				RotateBody -= f_RotateAmt;
			}
			else
			{
				b_Steer = false;
			}
			PlayerCar.v_UpdateCarDirection(RotateBody);
		}
	PlayerCar.v_UpdateCarSpeed(b_StepAccelerator, b_StepBrakes, b_Steer, dt);
	V_UpdatedPlayerPos = PlayerCar.V_UpdateCarPos(dt);

	TranslateBodyX = V_UpdatedPlayerPos.x;
	TranslateBodyY = V_UpdatedPlayerPos.y;
	TranslateBodyZ = V_UpdatedPlayerPos.z;
	
	// e[0].v_UpdateEnemyCarDirection(RotateEnemyBody,0);
	e[0].E_carspeed(b_StepENEMYAccelerator, b_StepENEMYBrakes, b_ENEMYSteer, dt);
	enemyUpdatePos[0] = e[0].V_UpdateenemyCarPos(dt);
	enemyX[0] = enemyUpdatePos[0].x;
	enemyY[0] = enemyUpdatePos[0].y;
	enemyZ[0] = enemyUpdatePos[0].z;

	Obj[OBJ_PLAYER]->setRotatingAxis(f_UpdatedAngle, 0.0f, 1.0f, 0.0f);
	Obj[OBJ_PLAYER]->setOBB(Vector3(TranslateBodyX, TranslateBodyY, TranslateBodyZ));

	for (int i = 0; i < 1; i++)
	{
		Obj[i+1]->setOBB(Vector3(enemyX[i], enemyY[i], enemyZ[i]));
	}

	//<collision>
	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[AllObjs]))
		{
			collide = true;
			i_CollidedWith = AllObjs;
			break;
		}
		collide = false;
	}

	if (collide)	//if it collides, what ever that was changed will be set to the previous frame
	{
		//PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));

		//if i_CollidedWith the numbers of Car AI
		//PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));
		//e[i_CollidedWith-1].v_SetEnemySpeed(-(e[i_CollidedWith-1].f_GetEnemySpeed() * 0.5));

		if (i_CollidedWith >= 1 /*&& i_CollidedWith <= last car AI*/) //num in object type
		{
			if (TranslateBodyZ > enemyZ[i_CollidedWith-1])
			{
				if ((TranslateBodyZ - enemyZ[i_CollidedWith - 1]) >= f_HeightAIP)	//if AI directly hits back of the car of player
				{
					PlayerCar.v_SetSpeed((fabs(PlayerCar.f_GetSpeed()) * 1.3));
					e[i_CollidedWith - 1].v_SetEnemySpeed(-(e[i_CollidedWith - 1].f_GetEnemySpeed() * 0.5));
				}
				else
				{
					PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));
					e[i_CollidedWith - 1].v_SetEnemySpeed(-(e[i_CollidedWith - 1].f_GetEnemySpeed() * 0.5));
				}
			}
			else
			{
				if ((enemyZ[i_CollidedWith - 1] - TranslateBodyZ) >= f_HeightAIP)	//if player directly hits back of the car of AI
				{
					PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));
					e[i_CollidedWith - 1].v_SetEnemySpeed((e[i_CollidedWith - 1].f_GetEnemySpeed() * 1.3));
				}
				else
				{
					PlayerCar.v_SetSpeed(-(PlayerCar.f_GetSpeed() * 0.5));
					e[i_CollidedWith - 1].v_SetEnemySpeed(-(e[i_CollidedWith - 1].f_GetEnemySpeed() * 0.5));
				}
			}
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
	
	if (getCurrentCam)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}

	if (Application::IsKeyPressed('Q'))
	{
		f_TPCRotateBy = -1.0f;
		//light[0].type = Light::LIGHT_POINT;  // For a lamp post
		//glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to POINT and pass the information to shader
	}
	else if (Application::IsKeyPressed('E'))
	{
		f_TPCRotateBy = 1.0f;
		//light[0].type = Light::LIGHT_DIRECTIONAL; // Used for smt like the sun, somewhere so far it shines on everything depending on angle
		//glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to DIRECTIONAL and pass the information to shader
	}
	else if (Application::IsKeyPressed('C'))
	{
		light[0].type = Light::LIGHT_SPOT; // For a torch light
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
		//to do: switch light type to SPOT and pass the information to shader
	}

	
	//camera.Update(dt);
	camera.Update(f_TPCRotateBy, TranslateBodyX, TranslateBodyY, TranslateBodyZ);
	f_TPCRotateBy = 0.0f;
}

void AssignmentScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderSkybox();

	//modelStack.PushMatrix();
	//modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ);
	//modelStack.Rotate(RotateBody, 0.0f, 1.0f, 0.0f);
	//RenderMesh(meshList[GEO_CUBE], false);
	//modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(TranslateBodyX, TranslateBodyY, TranslateBodyZ - 4);
	modelStack.Translate(0, 0, 4);
	modelStack.Rotate(RotateBody, 0.0f, 1.0f, 0.0f);
	modelStack.Translate(0, 0, -4);
	RenderMesh(meshList[GEO_AMBULANCE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(enemyX[0], enemyY[0], enemyZ[0]);
	RenderMesh(meshList[GEO_AICUBE], false);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(enemyX[0], enemyY[0], enemyZ[0]);
	////modelStack.Rotate(90, 0, 1, 0);
	//RenderMesh(meshList[GEO_CAR], false);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(enemyX[1], enemyY[1], enemyZ[1]);
	//modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[GEO_CAR], false);
	modelStack.PopMatrix();

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

	//<-----------Axes----------->
	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(8, 8, 8);
	modelStack.Translate(0, 8, 30);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[GEO_HOSPITAL], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(8, 8, 8);
	modelStack.Translate(0, 8, 20 /*22.5f*/);
	modelStack.Rotate(270, 0, 1, 0);
	RenderMesh(meshList[GEO_RACETRACK], false);
		modelStack.PushMatrix();
		modelStack.Translate(-9.6f, -0.01f, 0);
		RenderMesh(meshList[GEO_RACETRACK], false);
			modelStack.PushMatrix();
			modelStack.Translate(-9.6f, 0.01f, 0);
			RenderMesh(meshList[GEO_RACETRACK], false);
				modelStack.PushMatrix();
				modelStack.Translate(-9.6f, -0.01f, 0);
				RenderMesh(meshList[GEO_RACETRACK], false);
					modelStack.PushMatrix();
					modelStack.Translate(-9.6f, 0.01f, 0);
					RenderMesh(meshList[GEO_RACETRACK], false);
					modelStack.PopMatrix();
				modelStack.PopMatrix();
			modelStack.PopMatrix();
		modelStack.PopMatrix();
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
		//<--View stats for nerds-->
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("View stats:[P]"), Color(0, 0, 0), 2, 54, 58);
		modelStack.PopMatrix();
	}

	//<--Get cameras position-->
	//modelStack.PushMatrix();
	//RenderTextOnScreen(meshList[GEO_TEXT], ("Pos X:" + std::to_string(camera.position.x)+", Y:"+ std::to_string(camera.position.y) +" , Z:"+ std::to_string(camera.position.z)), Color(0, 1, 0), 2, 2, 5);
	//modelStack.PopMatrix();	

	int speed = fabs(PlayerCar.f_GetSpeed());

	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Speed:" + std::to_string(speed)), Color(0, 0, 0), 2, 2, 5);
	modelStack.PopMatrix();
	
	/*modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Tar X:" + std::to_string(camera.target.x)+", Y:"+ std::to_string(camera.target.y) +" , Z:"+ std::to_string(camera.target.z)), Color(1, 0, 0), 2, 2, 7);
	modelStack.PopMatrix();*/

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
	modelStack.Translate(0.0f, 0.2f, 0.0f);
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
