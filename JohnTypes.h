#pragma once

#include "base.h"

struct EntityToken
{
	UINT16 entityId;
	UINT16 x;
	UINT16 y;
	BYTE reserved[10];
	char* config;
};

struct WallToken
{
	UINT16 wallType;
	BYTE isDoor;
	BYTE lockType;
	UINT16 floor;
	UINT16 ceiling;
	BYTE zone;
	BYTE reserved[7];
	char* message;
};

struct LevelToken
{
	char watermark[3];
	BYTE version;
	UINT16 width;
	UINT16 height;
	UINT64 totalEntities;
	WallToken* walls;
	EntityToken* entities;
};