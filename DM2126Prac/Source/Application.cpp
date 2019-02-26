//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>
#include "Application.h"
#include "PuzzleRoom.h"
#include "Racing.h"
#include "Scene2.h"
#include "Leaderboard.h"
#include "TutorialPuzzleRoom.h"
#include "TutorialScene2.h"
#include "TutorialRacing.h"

#include "Sound.h"
#include "MainMenu.h"
#include "WinScreen.h"
#include "LoseScreen.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
double Application::timerh = 0;
//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
	SceneNumber = 0;
}

Application::~Application()
{
}


void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);//update opengl the new window size
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(1200, 800, "Test Window", NULL, NULL);

	//New to Scene 2
	glfwSetWindowSizeCallback(m_window, resize_callback);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int Application::GetSceneNumber()
{
	return SceneNumber;
}

int Application::SetSceneNumber(int number)
{
	SceneNumber = number;
	return SceneNumber;
}

void Application::Run()
{

	Scene *scene;
	//Main Loop
	if (GetSceneNumber()==1)
	{
		scene = new PuzzleRoom();
		glfwSetCursorPosCallback(m_window, PuzzleRoom::mouse_callback);// when ever the cursor moves, this function will be called
	}
	else if (GetSceneNumber()==2)
	{
		scene = new Scene2();
		glfwSetCursorPosCallback(m_window, Scene2::mouse_callback);// when ever the cursor moves, this function will be called
	}
	else if (GetSceneNumber() == 3)
	{
		scene = new RaceScene();
	}
	else if (GetSceneNumber() == 4)
	{
		scene = new TutorialPuzzleRoom();
	}
	else if (GetSceneNumber() == 5)
	{
		scene = new TutorialScene2();
	}
	else if (GetSceneNumber() == 6)
	{
		scene = new TutorialRaceScene();
	}
	else if (GetSceneNumber() == 7)	//Win screen
	{
		scene = new WinScene();
		//glfwSetCursorPosCallback(m_window, PuzzleRoom::mouse_callback);// when ever the cursor moves, this function will be called
	}
	else if (GetSceneNumber() == 8)	//Lose screen
	{
		scene = new LoseScene();	
	}
	else if (GetSceneNumber() == 9)	//Leaderboard screen
	{
		scene = new Leaderboard();	
	}
	else // change back to PuzzleRoom when pushing 
	{
		scene = new TutorialRaceScene();
		// glfwSetCursorPosCallback(m_window, Scene2::mouse_callback);// when ever the cursor moves, this function will be called

	}
	
	//Main menu scene = 0 no cam
	//Puzzle room = 1
	//Ambulance = 2
	//Race = 3
	//Instruction for PR = 4,5,6 if got multiple scene for instruction no cam
	//Win screen = 7 no cam
	//Lose = 8 no cam

	scene->Init();

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_END))
	{
		scene->Update(m_timer.getElapsedTime());
		scene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   
		if (Scene::b_GetEndGame())
		{
			break;
		}
	} //Check if the ESC key had been pressed or if the window had been closed
	scene->Exit();
	delete scene;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
