#include "GameManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"

#include "WorldObjectCBuffer.h"
#include "Shader.h"
#include "Level.h"
#include "IModel.h"
#include "Camera.h"
#include "JohnLoader.h"

std::unique_ptr<GameManager> GameManager::instance;


void GameManager::init()
{
	instance = std::make_unique<GameManager>();
}

GameManager::GameManager() :
	currentLevel(nullptr)
{

	currentLevel = new Level("Assets/Levels/Test.map");
	if (!currentLevel->loaded) {
		status = false;
		delete currentLevel;
		currentLevel = nullptr;
		return;
	}
	status = true;
}

GameManager::~GameManager()
{

}

void GameManager::update()
{
	if (ISDOWN('W')) {
		//GRAPHICS->camera->position = GRAPHICS->camera->rotation
		XMVECTOR camPos = XMLoadFloat3(&GRAPHICS->camera->position);
		XMVECTOR diff = XMVectorScale(GRAPHICS->camera->lookVec, 0.1f);
		camPos = XMVectorAdd(camPos, diff);
		XMStoreFloat3(&GRAPHICS->camera->position, camPos);
	}
}

void GameManager::draw()
{
	// draw starts here
	Shader* worldObjectShader = GRAPHICS->shaders["WorldObject"];
	worldObjectShader->use();

	auto buffer = (WorldObjectCBuffer*)worldObjectShader->cBuffer;
	buffer->entityPos = { -2, 0, 0, 0 };
	worldObjectShader->mapCBuffer();
	auto wiggle = GRAPHICS->models["Wiggle"];
	wiggle->draw();

	currentLevel->draw();
}