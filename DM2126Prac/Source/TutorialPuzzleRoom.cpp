#include "GL\glew.h"
#include "Mtx44.h"
#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "GLFW/glfw3.h"
#include "TutorialPuzzleRoom.h"
#include "Utility.h"
#include <string>
#include "Sound.h"

TutorialPuzzleRoom::TutorialPuzzleRoom()
{

}

TutorialPuzzleRoom::~TutorialPuzzleRoom()
{
	delete PuzzleTimer;
}
void TutorialPuzzleRoom::Init() //defines what shader to use
{
	//Background color
	glClearColor(0.0f, 0.14901960784f, 0.3f, 0.0f); //4 parameters (RGBA)
	PuzzleTimer = new StopWatchTimer;
	b_checkmodelStack = false;

	b_Switch = false;
	d_BounceTime = 0.25f;
	b_SwitchRotation = false;
	f_TPCRotateTotal = 0.0f;
	f_TPCRotateBy = 90.0f;

	music::player.init();
	music::player.setSoundVol(0.1);
	music::player.playSound("Sound//Other//InstructionBGM.wav", true);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	f_LSPEED = 30.0f;

	camera.Init(Vector3(-67.99, 92.75, -6.7), Vector3(-67.99, 52.8, -6.016), Vector3(0, 1, 0));

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
	
	meshList[GEO_HOUSE] = MeshBuilder::GenerateOBJ("House" ,"OBJ//house.obj");
	meshList[GEO_HOUSE]->textureID = LoadTGA("Image//house.tga");

	meshList[GEO_ROOM1WALL] = MeshBuilder::GenerateOBJ("Room1Wall", "OBJ//room1walls.obj");
	meshList[GEO_ROOM1WALL]->textureID = LoadTGA("Image//woodwall.tga");
	

	meshList[GEO_DOORTOROOM1] = MeshBuilder::GenerateOBJ("Room1Wall", "OBJ//doortoroom12.obj");
	meshList[GEO_DOORTOROOM1]->textureID = LoadTGA("Image//DoorTextures.tga");

	meshList[GEO_SOFA] = MeshBuilder::GenerateOBJ("Sofa", "OBJ//sofa2.obj");
	meshList[GEO_SOFA]->textureID = LoadTGA("Image//sofatxt.tga");
	
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	meshList[GEO_PAINTINGFRUIT] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//fruitpainting.obj");
	meshList[GEO_PAINTINGFRUIT]->textureID = LoadTGA("Image//fruitpainting.tga");
	
	meshList[GEO_PAINTINGCASTLE] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//castlepainting.obj");
	meshList[GEO_PAINTINGCASTLE]->textureID = LoadTGA("Image//castlepainting.tga");
	
	meshList[GEO_PAINTINGKEY] = MeshBuilder::GenerateOBJ("fruit painting", "OBJ//keypainting.obj");
	meshList[GEO_PAINTINGKEY]->textureID = LoadTGA("Image//keypainting.tga");

	meshList[GEO_PAINTINGFOREST] = MeshBuilder::GenerateOBJ("forestpainting", "OBJ//forestpainting.obj");
	meshList[GEO_PAINTINGFOREST]->textureID = LoadTGA("Image//forestpainting.tga");
	
	meshList[GEO_DOORENTRANCE] = MeshBuilder::GenerateOBJ("doorentrance", "OBJ//doorentrance.obj");
	meshList[GEO_DOORENTRANCE]->textureID = LoadTGA("Image//DoorTextures.tga");
	
	meshList[GEO_TV] = MeshBuilder::GenerateOBJ("TV", "OBJ//tv.obj");
	meshList[GEO_TV]->textureID = LoadTGA("Image//TVTextures.tga");

	meshList[GEO_TVTABLE] = MeshBuilder::GenerateOBJ("TV", "OBJ//tvtable.obj");
	meshList[GEO_TVTABLE]->textureID=LoadTGA("Image//TVTableTextures.tga");
	

	meshList[GEO_TVTABLEDRAWER] = MeshBuilder::GenerateOBJ("TVTableDrawer", "OBJ//tvtabledrawer.obj");
	meshList[GEO_TVTABLEDRAWER]->textureID = LoadTGA("Image//TVTableDrawerTextures.tga");
	
	meshList[GEO_BOOKSHELF] = MeshBuilder::GenerateOBJ("BookShelve", "OBJ//bookshelf.obj");
	meshList[GEO_BOOKSHELF]->textureID = LoadTGA("Image//BookShelfTexture.tga");

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

	meshList[GEO_PILLOW] = MeshBuilder::GenerateOBJ("Book1", "OBJ//pillow.obj");
	meshList[GEO_PILLOW]->textureID = LoadTGA("Image//Pillow.tga");
	
	meshList[GEO_SECRETWALL] = MeshBuilder::GenerateOBJ("Book1", "OBJ//secretwall.obj");
	meshList[GEO_SECRETWALL]->textureID = LoadTGA("Image//SecretWall.tga");

	meshList[GEO_SAFE] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safebox.obj");
	meshList[GEO_SAFE]->textureID = LoadTGA("Image//SafeBox.tga");

	meshList[GEO_SAFEDOOR] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safedoor.obj");
	meshList[GEO_SAFEDOOR]->textureID = LoadTGA("Image//SafeDoor.tga");

	meshList[GEO_KEY1] = MeshBuilder::GenerateOBJ("Key1,", "OBJ//key1.obj");
	meshList[GEO_KEY1]->textureID = LoadTGA("Image//Key1.tga");

	meshList[GEO_TABLEPAINTING] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//tablepainting.obj");
	meshList[GEO_TABLEPAINTING]->textureID = LoadTGA("Image//TablePainting.tga");

	meshList[GEO_SWITCHONE] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//switchone.obj");
	meshList[GEO_SWITCHONE]->textureID = LoadTGA("Image//Pillow.tga"); 
																			  
	meshList[GEO_SWITCHTWO] = MeshBuilder::GenerateOBJ("tablepainting", "OBJ//switchtwo.obj");
	meshList[GEO_SWITCHTWO]->textureID = LoadTGA("Image//Pillow.tga");

	meshList[GEO_KEY2] = MeshBuilder::GenerateOBJ("Key1,", "OBJ//key2.obj");
	meshList[GEO_KEY2]->textureID = LoadTGA("Image//Key1.tga");

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

	meshList[GEO_KEY1]->material.kShininess = 1.0f;

	// Switching Stage
	meshList[GEO_START] = MeshBuilder::GenerateQuad("Stage", Color(0, 0, 1), 22, 22, 0);
	meshList[GEO_START]->textureID = LoadTGA("Image//T1P1redo.tga");
}

void TutorialPuzzleRoom::Update(double dt)
{
	if (!b_SwitchRotation)
	{
		f_TPCRotateBy += 0.5f;
		f_TPCRotateTotal += 0.5f;
	}
	else
	{
		f_TPCRotateBy -= 0.5f;
		f_TPCRotateTotal -= 0.5f;
	}


	if (f_TPCRotateTotal > 190.0f)
	{
		b_SwitchRotation = true;
	}
	else if (f_TPCRotateTotal < 0.0f)
	{
		b_SwitchRotation = false;
	}

	d_BounceTime -= dt;

	if (Application::IsKeyPressed(VK_RIGHT) && d_BounceTime < 0.0f)
	{
		Application app;
		app.SetSceneNumber(5);
		app.Run();
		d_BounceTime = 0.25f;
	}

	if (Application::IsKeyPressed(VK_ESCAPE))
	{
		music::player.stopSound();

		Application app;
		app.SetSceneNumber(0);
		app.Run();
	}


	camera.Update(f_TPCRotateBy, 30, 30, 6);
	f_TPCRotateBy = 0.0f;
}


void TutorialPuzzleRoom::Render()
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

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL1], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL2], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	DrawHUD(meshList[GEO_START], Color(0, 0, 1), false, 1, 40, 30);
	modelStack.PopMatrix();

}

void TutorialPuzzleRoom::RenderMesh(Mesh *mesh, bool enableLight)
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

void TutorialPuzzleRoom::RenderSkybox()
{

}

void TutorialPuzzleRoom::CreepyHouse()
{
	//ALL MUST BE BELOW THIS
	//Scale
	modelStack.PushMatrix();
	modelStack.Scale(10.f,10.f,10.f);
	modelStack.PushMatrix();
	if (b_havepatient == false)
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
	//Switches
	RenderMesh(meshList[GEO_SWITCHONE], true);
	RenderMesh(meshList[GEO_SWITCHTWO], true);

	//TablePainting
	RenderMesh(meshList[GEO_TABLEPAINTING], true);
	//Safe
	RenderMesh(meshList[GEO_SAFE], true);
	RenderMesh(meshList[GEO_SAFEDOOR], true);
	//SecretWall
	modelStack.PushMatrix();
	modelStack.Translate(0,-30 + f_secretdoortranslation,0);
	RenderMesh(meshList[GEO_SECRETWALL], true);
	modelStack.PopMatrix();


	//BedSet
	RenderMesh(meshList[GEO_MATTRESS], true);
	RenderMesh(meshList[GEO_BEDFRAME], true);
	
	modelStack.PushMatrix();
	modelStack.Translate(0 + f_pillowtranslation, 0, 0);
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
	modelStack.Translate(0 - f_drawertranslation,0,0);
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
	modelStack.Rotate(f_RotateDoor1, 0, 1, 0);
	modelStack.Translate(-7.93, -5, 2.08);
	RenderMesh(meshList[GEO_DOORTOROOM1],true);
	modelStack.PopMatrix();
	
	//RoomWall1
	RenderMesh(meshList[GEO_ROOM1WALL], true);


	//house
	RenderMesh(meshList[GEO_HOUSE], true);

	modelStack.PopMatrix();

}

void TutorialPuzzleRoom::RenderText(Mesh* mesh, std::string text, Color color)
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

void TutorialPuzzleRoom::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
void TutorialPuzzleRoom::DrawHUD(Mesh* mesh, Color color, bool enableLight, float size, float x, float y)
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
void TutorialPuzzleRoom::Exit()
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