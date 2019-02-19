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

float PuzzleRoom::lastX = 0.0f;
float PuzzleRoom::lastY = 0.0f;
Camera2 PuzzleRoom::camera = Camera2();


PuzzleRoom::PuzzleRoom()
{
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
}
void PuzzleRoom::Init() //defines what shader to use
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

	glUseProgram(m_programID);

	light[0].type = Light::LIGHT_POINT; //light of the room
	light[0].position.Set(0.5, 89.75, 35.9);//position of light
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
	Obj[OBJ_PLAYER] = new ObjectBox(Vector3(camera.position.x, camera.position.y, camera.position.z), 10.f, 25.f, 10.f);
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
	//<----BMO body---->
	meshList[GEO_SOFA] = MeshBuilder::GenerateOBJ("Sofa", "OBJ//sofa.obj");
	meshList[GEO_SOFA]->textureID = LoadTGA("Image//sofatxt.tga");
	Obj[OBJ_SOFA] = new ObjectBox(Vector3(39, 58.50, 37), 19.5, 65, 44.9);


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
	Obj[OBJ_TABLE] = new ObjectBox(Vector3(94.5, 58.50, 35.7), 7, 65, 19.1);


	meshList[GEO_TVTABLEDRAWER] = MeshBuilder::GenerateOBJ("TVTableDrawer", "OBJ//tvtabledrawer.obj");
	meshList[GEO_TVTABLEDRAWER]->textureID = LoadTGA("Image//TVTableDrawerTextures.tga");

	meshList[GEO_BOOKSHELF] = MeshBuilder::GenerateOBJ("BookShelve", "OBJ//bookshelf.obj");
	meshList[GEO_BOOKSHELF]->textureID = LoadTGA("Image//BookShelfTexture.tga");
	Obj[OBJ_BOOKSHELF] = new ObjectBox(Vector3(90.23, 58.50, -37.04),29.08, 65, 8.49);

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
	meshList[GEO_MATTRESS]->textureID = LoadTGA("Image//.tga");

	meshList[GEO_BEDFRAME] = MeshBuilder::GenerateOBJ("Book1", "OBJ//bedframe.obj");
	meshList[GEO_BEDFRAME]->textureID = LoadTGA("Image//BedFrame.tga");
	Obj[OBJ_BED] = new ObjectBox(Vector3(34, 58.50, -104.03), 45.03, 65, 31.82);


	meshList[GEO_PILLOW] = MeshBuilder::GenerateOBJ("Book1", "OBJ//pillow.obj");
	meshList[GEO_PILLOW]->textureID = LoadTGA("Image//Pillow.tga");

	meshList[GEO_SECRETWALL] = MeshBuilder::GenerateOBJ("Book1", "OBJ//secretwall.obj");
	meshList[GEO_SECRETWALL]->textureID = LoadTGA("Image//SecretWall.tga");

	meshList[GEO_SAFE] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safebox.obj");
	meshList[GEO_SAFE]->textureID = LoadTGA("Image//SafeBox.tga");
	Obj[OBJ_SAFE] = new ObjectBox(Vector3(110.55, 58.50, 135.23), 48, 65, 48);


	meshList[GEO_SAFEDOOR] = MeshBuilder::GenerateOBJ("Book1", "OBJ//safedoor.obj");
	meshList[GEO_SAFEDOOR]->textureID = LoadTGA("Image//SafeDoor.tga");

	meshList[GEO_KEY] = MeshBuilder::GenerateOBJ("Key1,", "OBJ//key1.obj");
	meshList[GEO_KEY]->textureID = LoadTGA("Image//Key1.tga");
	/*meshList[GEO_KEY]->material.kAmbient.Set(0.00f, 0.00f, 0.00f);
	meshList[GEO_KEY]->material.kDiffuse.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_KEY]->material.kSpecular.Set(0.3f, 0.3f, 0.3f);
 */ 
	meshList[GEO_KEY]->material.kShininess = 1.0f;
}

void PuzzleRoom::PlayMusic()
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
	else if (!b_inPC && !b_inPM)
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

void PuzzleRoom::Update(double dt)
{
	elapsedtime += dt;
	if (collide) 
	{
		modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("Press E to interact"), Color(0, 0, 0), 2, 54, 58);
		modelStack.PopMatrix();
		if (Application::IsKeyPressed('E'))
		{
			if (elapsedtime > bouncetime)
			{

				door1open = !door1open;
				bouncetime = elapsedtime + 0.2f;
			}
		}
	}
	if (door1open && RotateDoor1< 85)
	{
		Obj[OBJ_DOOR] = new ObjectBox(Vector3(55.7, 58.50, -14.5), 31.82, 65, 4);
		RotateDoor1 += 15 * dt * 2;
	}
	else if (!door1open && RotateDoor1 > 0)
	{
		Obj[OBJ_DOOR] = new ObjectBox(Vector3(68.3, 58.50, -31.28), 4, 65, 31.82);
		RotateDoor1 -= 15 * dt * 2;
	}
	if (Application::IsKeyPressed('2'))
	{
		Application app;
		app.SetSceneNumber(2);
		app.Run();
	}
	if (Application::IsKeyPressed('3'))
	{
		Application app;
		app.SetSceneNumber(3);
		app.Run();
	}
	if (Application::IsKeyPressed('4'))
	{
		Application app;
		app.SetSceneNumber(4);
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

	fps = 1.0f / (float)dt;



	//<--Walking animation-->
	if (checkmodelStack)
	{
		if (running)
		{
			ArmRotation -= (float)(dt * 3) * 90;
			LeftLegX += (float)(dt) * 50;
			RightLegX -= (float)(dt) * 50;
			if (LeftLegX > 100)
			{
				running = false;
			}
		}
		else
		{
			ArmRotation += (float)(dt * 3) * 90;
			LeftLegX -= (float)(dt) * 50;
			RightLegX += (float)(dt) * 50;
			if (LeftLegX < 80)
			{
				running = true;
			}
		}

		if (bodyMovement)
		{
			TranslateBodyY += (float)(dt * 5);
			if (TranslateBodyY > 16.0f)
			{
				bodyMovement = false;
			}
		}
		else {
			TranslateBodyY -= (float)(dt * 5);
			if (TranslateBodyY <= 15.0f)
			{
				bodyMovement = true;
			}
		}
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
	
	Obj[OBJ_PLAYER]->setOBB(Vector3(camera.position.x, camera.position.y, camera.position.z));
	for (int AllObjs = 1; AllObjs < NUM_OBJ; ++AllObjs)
	{
		if (ObjectBox::checkCollision(*Obj[OBJ_PLAYER], *Obj[AllObjs]))
		{
			if (AllObjs == 7)
			{
				//do smt
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
	PlayMusic();
	camera.Update(dt);
}

void PuzzleRoom::Render()
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

	////<-----------Axes----------->
	//modelStack.PushMatrix();
	//RenderMesh(meshList[GEO_AXES], false);
	//modelStack.PopMatrix();

	//<-----------Light ball Sphere lighting 1----------->
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_LIGHTBALL], false);
	modelStack.PopMatrix();

	//<--BMO-->
	if (b_BMO)
	{

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
		/*modelStack.PushMatrix();
		RenderTextOnScreen(meshList[GEO_TEXT], ("View stats:[P]"), Color(0, 0, 0), 2, 54, 58);
		modelStack.PopMatrix();*/
	}

	//<--Get cameras position-->
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Pos X:" + std::to_string(camera.position.x)+", Y:"+ std::to_string(camera.position.y) +" , Z:"+ std::to_string(camera.position.z)), Color(0, 1, 0), 2, 2, 5);
	modelStack.PopMatrix();
	
	modelStack.PushMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], ("Tar X:" + std::to_string(camera.target.x)+", Y:"+ std::to_string(camera.target.y) +" , Z:"+ std::to_string(camera.target.z)), Color(1, 0, 0), 2, 2, 7);
	 modelStack.PopMatrix();



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

void PuzzleRoom::RenderPokeCenter()
{
	//ALL MUST BE BELOW THIS
	//Scale
	modelStack.PushMatrix();
	modelStack.Scale(10.f,10.f,10.f);
	//key
	RenderMesh(meshList[GEO_KEY], true);
	//Safe
	RenderMesh(meshList[GEO_SAFE], true);
	RenderMesh(meshList[GEO_SAFEDOOR], true);
	//SecretWall
	RenderMesh(meshList[GEO_SECRETWALL], true);

	//BedSet
	RenderMesh(meshList[GEO_MATTRESS], true);
	RenderMesh(meshList[GEO_BEDFRAME], true);
	RenderMesh(meshList[GEO_PILLOW], true);

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
	RenderMesh(meshList[GEO_TVTABLEDRAWER], true);
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

	/*
	modelStack.PushMatrix();
	modelStack.Scale(2.f, 2.f, 2.f);
	modelStack.PushMatrix();
	modelStack.Scale(100.0f*2.5, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.0f, -0.98f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_WALLS], true); //back
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f*2.5, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.0f, 0.98f);
	modelStack.Rotate(180, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_WALLS], true); //front
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(2.5f, 1.f, 1.f);
	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 1.5f, 0.0f);
	modelStack.Rotate(180, 0.0f, 0.0f, 1.0f);
	modelStack.Rotate(270, 0.0f, 1.0f, 0.0f);
	RenderMesh(meshList[GEO_ROOF], true); //top
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_FLOOR], true); //bottom
	modelStack.PopMatrix();
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(-0.98*2.5f, 1.0f, 0.0f);
	modelStack.Rotate(90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_WALLS], true); //left
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(100.0f, 100.0f, 100.0f);
	modelStack.Translate(0.98*2.5f, 1.0f, 0.0f);
	modelStack.Rotate(-90, 0.0f, 1.0f, 0.0f);
	modelStack.Rotate(90, 1.0f, 0.0f, 0.0f);
	RenderMesh(meshList[GEO_WALLS], true); //right
	modelStack.PopMatrix();
	modelStack.PopMatrix();
	*/
}

void PuzzleRoom::RenderButton(int geo_circle, int geo_cylinder)
{
	//<----Button circle---->
	RenderMesh(meshList[geo_circle], false);

	//<----Button cylinder---->
	modelStack.PushMatrix();
	RenderMesh(meshList[geo_cylinder], false);
	modelStack.PopMatrix();
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