#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "TutorialScene2.h"
#include "Utility.h"
#include <string>
#include "Sound.h"

TutorialScene2::TutorialScene2()
{
	
}

TutorialScene2::~TutorialScene2()
{

}

void TutorialScene2::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)
	setTrueFalse = false;
	b_Switch = false;
	d_BounceTime = 0.25f;

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LSPEED = 30.0f;

	// Change here for camera initial position
	camera.Init(Vector3(0, 20, 6), Vector3(-10, 0, 0), Vector3(0, 1, 0));

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

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//WallWindow.tga");

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//WallExit.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//Wall.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//Wall.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.0f, 0.0f, 1.0f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//Wall.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	// Stuff added so far
	meshList[GEO_STRETCHER] = MeshBuilder::GenerateOBJ("Stretcher", "OBJ//Stretcher.obj");
	meshList[GEO_STRETCHER]->textureID = LoadTGA("Image//Stretcher.tga"); 

	meshList[GEO_DEFIBRILLATOR] = MeshBuilder::GenerateOBJ("Defibrillator", "OBJ//Defibrillator2.obj");
	meshList[GEO_DEFIBRILLATOR]->textureID = LoadTGA("Image//Defibrillator2.tga");

	meshList[GEO_FIRSTAIDKIT] = MeshBuilder::GenerateOBJ("FirstAidKit", "OBJ//FirstAidKit.obj");
	meshList[GEO_FIRSTAIDKIT]->textureID = LoadTGA("Image//FirstAidKit.tga");

	meshList[GEO_CABINET] = MeshBuilder::GenerateOBJ("Cabinet", "OBJ//Cabinet.obj"); // main cabinet
	meshList[GEO_CABINET]->textureID = LoadTGA("Image//Cabinet.tga"); 

	meshList[GEO_CABINET2] = MeshBuilder::GenerateOBJ("Cabinet2", "OBJ//Cabinet2Working.obj"); // med cabinet
	meshList[GEO_CABINET2]->textureID = LoadTGA("Image//Cabinet2Working.tga"); 

	meshList[GEO_CHAIR] = MeshBuilder::GenerateOBJ("Chair", "OBJ//Chair.obj");
	meshList[GEO_CHAIR]->textureID = LoadTGA("Image//Chair.tga");

	meshList[GEO_TOPSHELVE1] = MeshBuilder::GenerateOBJ("TopShelve1", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE1]->textureID = LoadTGA("Image//TopShelve1.tga");

	meshList[GEO_TOPSHELVE2] = MeshBuilder::GenerateOBJ("TopShelve2", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE2]->textureID = LoadTGA("Image//TopShelve2.tga");

	meshList[GEO_TOPSHELVE3] = MeshBuilder::GenerateOBJ("TopShelve3", "OBJ//TopShelve.obj");
	meshList[GEO_TOPSHELVE3]->textureID = LoadTGA("Image//TopShelve3.tga");

	meshList[GEO_METALSHELVE] = MeshBuilder::GenerateOBJ("MetalShelve", "OBJ//MetalShelve2.obj");
	meshList[GEO_METALSHELVE]->textureID = LoadTGA("Image//MetalShelveNew.tga");

	// Collision Box for Camera/Player -> brot from A2 Scene
	meshList[GEO_PLAYER] = MeshBuilder::GenerateCube("Box", Color(0, 0, 1), 3.0f, 3.0f, 3.0f); // 10, 15, 15

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
	meshList[GEO_START] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 22, 22, 0);
	meshList[GEO_START]->textureID = LoadTGA("Image//TutScene2Pt1.tga");

	meshList[GEO_START2] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 22, 22, 0);
	meshList[GEO_START2]->textureID = LoadTGA("Image//TutScene2Pt2.tga");
}


void TutorialScene2::Update(double dt)
{
	f_TPCRotateBy += 0.5f;

	d_BounceTime -= dt;

	if (Application::IsKeyPressed(VK_LEFT) && d_BounceTime < 0.0f)
	{
		if (b_Switch)
			b_Switch = false;
		else
			b_Switch = true;

		d_BounceTime = 0.25f;
	}
	else if (Application::IsKeyPressed(VK_RIGHT) && d_BounceTime < 0.0f)
	{
		if (b_Switch)
			b_Switch = false;
		else
			b_Switch = true;
		d_BounceTime = 0.25f;
	}

	if (Application::IsKeyPressed(VK_ESCAPE))
	{
		Application app;
		app.SetSceneNumber(0);
		app.Run();
	}
	

	camera.Update(f_TPCRotateBy, 0, 20, 6);
	f_TPCRotateBy = 0.0f;
}

void TutorialScene2::Render()
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
	if (b_Switch)
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



void TutorialScene2::RenderMesh(Mesh *mesh, bool enableLight)
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

void TutorialScene2::RenderSkybox()
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

void TutorialScene2::RenderText(Mesh* mesh, std::string text, Color color)
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

void TutorialScene2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void TutorialScene2::DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y)
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

void TutorialScene2::Exit()
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
