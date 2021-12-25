#pragma once

#include <memory>
#include "GlobalCBuffer.h"

#define GAME (GameManager::instance)
#define LEVEL (GAME->currentLevel)

//forward declarations
class Level;

class GameManager
{
public:
	static std::unique_ptr<GameManager> instance;
	static void init();

	GameManager();
	~GameManager();

	Level* currentLevel;

	bool status;

	void update();
	void draw();
};

