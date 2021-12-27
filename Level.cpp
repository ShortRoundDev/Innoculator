#include "Level.h"

#include "JohnLoader.h"
#include "Wall.h"
#include "FloorCeiling.h"

Level::Level(std::string path):
	height(0),
	width(0),
	player(nullptr),
	walls(nullptr),
	loaded(true),
	levelToken(nullptr)
{
	JohnLoader loader = JohnLoader();
	BOOL success = loader.loadFile(path, &levelToken);
	if (!success) {
		loaded = false;
		return;
	}
	
	width = levelToken->width;
	height = levelToken->height;

	initWalls(levelToken);
}

Level::~Level()
{
	if (levelToken != nullptr) {
		HeapFree(GetProcessHeap(), 0, levelToken);
	}
}

void Level::draw()
{
	for (int i = 0; i < height; i++) {
		Wall** wallRow = walls[i];
		FloorCeiling** floorRow = floors[i];
		FloorCeiling** ceilingRow = ceilings[i];

		for (int j = 0; j < width; j++) {
			Wall* wall = wallRow[j];
			if (wall) {
				wall->draw();
			}

			FloorCeiling* floor = floorRow[j];
			if (floor) {
				floor->draw();
			}

			FloorCeiling* ceiling = ceilingRow[j];
			if (ceiling) {
				ceiling->draw();
			}
		}
	}
}

void Level::update()
{

}

void Level::initWalls(LevelToken* token)
{
	if (height == 0 || width == 0) {
		loaded = false;
		return;
	}

	walls		= (Wall***)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Wall**) * height);
	floors		= (FloorCeiling***)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FloorCeiling**) * height);
	ceilings	= (FloorCeiling***)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FloorCeiling**) * height);

	if (walls == NULL || floors == NULL || ceilings == NULL) {
		loaded = false;
		return;
	}
	
	for (int i = 0; i < height; i++) {
		walls[i]	= (Wall**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Wall*) * width);
		floors[i]	= (FloorCeiling**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FloorCeiling*) * width);
		ceilings[i] = (FloorCeiling**)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FloorCeiling*) * width);

		if (walls[i] == NULL || floors[i] == NULL || ceilings[i] == NULL) {
			HeapFree(GetProcessHeap(), 0, walls);
			HeapFree(GetProcessHeap(), 0, ceilings);
			HeapFree(GetProcessHeap(), 0, floors);

			loaded = false;
			return;
		
		}
		for (int j = 0; j < width; j++) {
			WallToken* wallToken = token->walls + (j + (i * width));

			if (wallToken->wallType != 0) {
				walls[i][j] = new Wall(wallToken, (UINT16)j, (UINT16)i);
			}
			
			if (wallToken->ceiling != 0) {
				ceilings[i][j] = new FloorCeiling(wallToken, false, (UINT16)j, (UINT16)i);
			}

			if (wallToken->floor != 0) {
				floors[i][j] = new FloorCeiling(wallToken, true, (UINT16)j, (UINT16)i);
			}
		}
	}
}
