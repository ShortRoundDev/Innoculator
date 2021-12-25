#include "GameManager.h"
#include "GraphicsManager.h"

#include "WorldObjectCBuffer.h"
#include "Shader.h"
#include "Level.h"
#include "Model.h"
#include "JohnLoader.h"

std::unique_ptr<GameManager> GameManager::instance;


void GameManager::init()
{
	instance = std::make_unique<GameManager>();
}

GameManager::GameManager() :
	currentLevel(nullptr)
{

	JohnLoader loader = JohnLoader();
	LevelToken* level = NULL;
	BOOL success = loader.loadFile("Assets/Levels/test.map", &level);
	if (!success) {
		status = false;
		return;
	}
	status = true;
}

GameManager::~GameManager()
{

}

void GameManager::update()
{

}

void GameManager::draw()
{
	// draw starts here
	Shader* worldObjectShader = GRAPHICS->shaders["WorldObject"];
	worldObjectShader->use();

	WorldObjectCBuffer* cBuffer = (WorldObjectCBuffer*)(worldObjectShader->cBuffer);
	cBuffer->entityPos = { 0, 0, 3, 0 };

	worldObjectShader->mapCBuffer();

	auto wall = GRAPHICS->getModel("Wall", "Assets/Models/Walls/Wall.obj");
	if (wall != NULL) {
		wall->draw();
	}
}