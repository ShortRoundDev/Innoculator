#pragma once

#include <vector>
#include <string>

#include "base.h"
#include "JohnTypes.h"

//forward declarations:

class Entity;
class Wall;
class FloorCeiling;

class Level
{
public:
	Level(std::string path);
	~Level();

	bool loaded;

	LevelToken* levelToken;

	std::vector<Entity*> entities;
	Entity* player;
	Wall*** walls;
	FloorCeiling*** floors;
	FloorCeiling*** ceilings;

	UINT16 width;
	UINT16 height;

	void draw();
	void update();

private:
	void initWalls(LevelToken* token);
};
