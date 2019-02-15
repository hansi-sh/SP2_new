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

void AssignmentScene::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)

	checkmodelStack = false;
	running = true;
	bodyMovement = true;
	b_BMO = true;
	b_viewStats = false;

	//<----for BMO body animation movement when running---->
	LeftLegX = 90.0f;
	RightLegX = 90.0f;
	ArmRotation = 0.0f;
	TranslateBodyX = 0.0f;
	TranslateBodyY = 15.0f;
	TranslateBodyZ = 0.0f;
	RotateBody = 0.0f;

	//<--Music-->
	b_musicSelected = false;
	b_inPM = false;
	b_inPC = false;
	
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LSPEED = 30.0f;

	camera.Init(Vector3(0, 160, 80), Vector3(0, 0, 0), Vector3(0, 1, 0));

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
	//meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Reference", 1000.0f, 1000.0f, 1000.0f);

	////<--USB-->
	//meshList[GEO_USB] = MeshBuilder::GenerateCube("USB", Color(0.01171875f, 0.19140625f, 0.23046875f), 2.0f, 0.4f, 3.0f);
	//meshList[GEO_USB]->material.kAmbient.Set(0.7f, 0.7f, 0.7f);
	//meshList[GEO_USB]->material.kDiffuse.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_USB]->material.kSpecular.Set(0.2f, 0.2f, 0.2f);
	//meshList[GEO_USB]->material.kShininess = 1.0f;

	////<--Blue button circle-->
	//meshList[GEO_BLUEBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Blue button circle", Color(0.0f, 0.0f, 0.74296875f), 360, 1.0f);
	//meshList[GEO_BLUEBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_BLUEBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_BLUEBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_BLUEBUTTONCIRCLE]->material.kShininess = 1.0f;

	////<--Blue button cylinder-->
	//meshList[GEO_BLUEBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Blue button cylinder", Color(0.0f, 0.0f, 0.4f), 360.0f, 1.0f, -3.0f);
	//meshList[GEO_BLUEBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_BLUEBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_BLUEBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_BLUEBUTTONCYLINDER]->material.kShininess = 1.0f;

	////<--Green button circle-->
	//meshList[GEO_GREENBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Green button circle", Color(0.0f, 0.64296875f, 0.0f), 360, 1.0f);
	//meshList[GEO_GREENBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_GREENBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_GREENBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_GREENBUTTONCIRCLE]->material.kShininess = 1.0f;

	////<--Green button cylinder-->
	//meshList[GEO_GREENBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Green button cylinder", Color(0.0f, 0.4f, 0.0f), 360.0f, 1.0f, -3.0f);
	//meshList[GEO_GREENBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_GREENBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_GREENBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_GREENBUTTONCYLINDER]->material.kShininess = 1.0f;

	////<--Red button circle-->
	//meshList[GEO_REDBUTTONCIRCLE] = MeshBuilder::GenerateCircle("Red button circle", Color(0.64296875f, 0.0f, 0.0f), 360, 1.0f);
	//meshList[GEO_REDBUTTONCIRCLE]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_REDBUTTONCIRCLE]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_REDBUTTONCIRCLE]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_REDBUTTONCIRCLE]->material.kShininess = 1.0f;

	////<--Red button cylinder-->
	//meshList[GEO_REDBUTTONCYLINDER] = MeshBuilder::GenerateCylinder("Red button cylinder", Color(0.4f, 0.0f, 0.0f), 360.0f, 1.0f, -3.0f);
	//meshList[GEO_REDBUTTONCYLINDER]->material.kAmbient.Set(0.80f, 0.80f, 0.80f);
	//meshList[GEO_REDBUTTONCYLINDER]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_REDBUTTONCYLINDER]->material.kSpecular.Set(0.5f, 0.5f, 0.5f);
	//meshList[GEO_REDBUTTONCYLINDER]->material.kShininess = 1.0f;

	////<--Turquoise triangle button-->
	//meshList[GEO_TRIPRISM] = MeshBuilder::GeneratePrism("Triangular button", Color(0.0f, 0.8f, 0.8f), 1.0f, 1.0f, 5.0f);
	//meshList[GEO_TRIPRISM]->material.kAmbient.Set(0.80f, 0.80f, 0.80f); 
	//meshList[GEO_TRIPRISM]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f); 
	//meshList[GEO_TRIPRISM]->material.kSpecular.Set(0.5f, 0.5f, 0.5f); 
	//meshList[GEO_TRIPRISM]->material.kShininess = 1.0f;

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

	fps = 1.0f / (float)dt;

	// Light movement
	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('U'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y += (float)(LSPEED * dt);

	if (getCurrentCam)
	{
		currentCamPos = camera.position;
		currentCamTarget = camera.target;
	}

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

	
	camera.Update(dt);
}

void AssignmentScene::Render()
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

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	//	//<-----------USB----------->
	//	modelStack.PushMatrix();
	//	modelStack.Translate(-2.0f, -1.0f, 0.01f);
	//	RenderMesh(meshList[GEO_USB], false);
	//	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(8, 8, 8);
	modelStack.Translate(/*-34*/-12, 8, 0);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[GEO_HOSPITAL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(8, 8, 8);
	modelStack.Translate(/*-22.5f*/ -0.5f, 8, 0);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[GEO_RACETRACK], true);
		//modelStack.PushMatrix();
		//modelStack.Translate(-11, -0.01f, 0);
		//RenderMesh(meshList[GEO_RACETRACK], true);
		//	modelStack.PushMatrix();
		//	modelStack.Translate(-11, 0.01f, 0);
		//	RenderMesh(meshList[GEO_RACETRACK], true);
		//		modelStack.PushMatrix();
		//		modelStack.Translate(-11, 0.01f, 0);
		//		RenderMesh(meshList[GEO_RACETRACK], true);
		//			modelStack.PushMatrix();
		//			modelStack.Translate(-11, 0.01f, 0);
		//			RenderMesh(meshList[GEO_RACETRACK], true);
		//			modelStack.PopMatrix();
		//		modelStack.PopMatrix();
		//	modelStack.PopMatrix();
		//modelStack.PopMatrix();
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

	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("PLayer:BMO"), Color(1, 1, 1), 3, 2, 55);
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
	modelStack.Translate(0.0f, 0.96f, -1.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 0.96f, 1.0f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(0.0f, 1.96f, 0.0f);
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
	modelStack.Translate(-1.0f, 0.96f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(SKYBOXSIZE, SKYBOXSIZE, SKYBOXSIZE);
	modelStack.Translate(1.0f, 0.96f, 0.0f);
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
