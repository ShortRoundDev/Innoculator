#include "FloorCeiling.h"

#include "GraphicsManager.h"

#include "WorldObjectCBuffer.h"
#include "Texture.h"
#include "IModel.h"
#include "Shader.h"

FloorCeiling::FloorCeiling(WallToken* token, bool isFloor, UINT16 x, UINT16 y) :
	model(GRAPHICS->getModel("Plane", "Plane")),
	shader(GRAPHICS->shaders["WorldObject"]),
	texture(nullptr)
{
	texture = getTexture(
		isFloor
		? token->floor
		: token->ceiling
	);
	float height = isFloor ? 0.0f : 1.0f;
	position = { (float)x, height, (float)y, 0.0f };
}

FloorCeiling::~FloorCeiling()
{

}

void FloorCeiling::draw()
{
	WorldObjectCBuffer* cbuffer = (WorldObjectCBuffer*)shader->cBuffer;
	cbuffer->entityPos = position;
	shader->mapCBuffer();

	if (texture) {
		model->draw(texture);
	}
	else {
		model->draw();
	}
}

void FloorCeiling::update()
{

}

Texture* FloorCeiling::getTexture(UINT16 tile)
{
	std::string wallTex = std::to_string(tile);

	return GRAPHICS->getTexture(
		wallTex,
		"Assets/Textures/Walls/" + wallTex + ".png"
	);
}