#pragma once

#include "base.h"
#include "JohnTypes.h"

#include <DirectXMath.h>

class Texture;
class IModel;
class Shader;

using namespace DirectX;

class FloorCeiling
{
public:
	FloorCeiling(WallToken* token, bool isFloor, UINT16 x, UINT16 y);
	~FloorCeiling();

	Texture* texture;
	IModel* model;
	Shader* shader;

	XMFLOAT4 position;

	void draw();
	void update();

private:
	Texture* getTexture(UINT16 tile);

};

