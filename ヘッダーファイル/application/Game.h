#pragma once
#include <Windows.h>
#include "Framework.h"


class Camera;
class Sprite;
class Object3d;
class ParticleEmitter;
class Skybox;


class Game : public Framework
{

public:

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;
	
	

private:

	

	Camera* camera = nullptr;

	

	

	Sprite* sprite = nullptr;	

	Object3d* object[2] = { nullptr };


	ParticleEmitter* particleCircle = nullptr;
	ParticleEmitter* particleRing = nullptr;
	


	Skybox* skybox = nullptr;

};
