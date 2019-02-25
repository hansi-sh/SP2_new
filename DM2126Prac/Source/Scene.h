#ifndef SCENE_H
#define SCENE_H

class Scene
{
protected:
	static bool ToEndGame;
public:
	Scene();
	~Scene();

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
	static bool b_GetEndGame();
};

#endif