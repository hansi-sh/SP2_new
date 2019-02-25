#include "Scene.h"

bool Scene::ToEndGame = false;

Scene::Scene()
{
}

Scene::~Scene()
{

}

bool Scene::b_GetEndGame()
{
	return ToEndGame;
}