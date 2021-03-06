#pragma once

#include "base.h"
#include "JohnTypes.h"

class IModel;
#include <DirectXMath.h>

using namespace DirectX;

class Shader;
class Texture;
class Model;

class Wall
{
public:
	Wall(WallToken* token, UINT x, UINT y);
	~Wall();
		
	bool isDoor;
	IModel* model;
	Texture* texture;
	Shader* shader;
	BYTE zone;
	char* message;

	XMFLOAT4 position;
	void draw();
	void update();

private:
	Texture* getTexture(UINT16 tile);
};

