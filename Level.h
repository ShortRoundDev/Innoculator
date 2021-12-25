#pragma once

#include <vector>
#include <string>

#include "base.h"

//forward declarations:

class Entity;
class Wall;

class Level
{
public:
	Level(std::string path);
	~Level();

	bool loaded;

	std::vector<Entity*> entities;
	Entity* player;
	Wall** walls;

	UINT16 width;
	UINT16 height;

	void draw();
	void update();

private:
	void initWalls(UINT16 width, UINT16 height);
};
