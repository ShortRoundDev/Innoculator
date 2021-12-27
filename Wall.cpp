#include "Wall.h"

#include "GraphicsManager.h"
#include "Texture.h"
#include "IModel.h"
#include "Shader.h"
#include "WorldObjectCBuffer.h"

Wall::Wall(WallToken* token, UINT x, UINT y):
	model(GRAPHICS->getModel("Wall", "Wall")),
	texture(getTexture(token->wallType)),
	shader(GRAPHICS->shaders["WorldObject"]),
	message(token->message),
	zone(token->zone),
	position({ (float)x, 0.0f, (float)y, 0.0f })
{
}

Wall::~Wall()
{

}

Texture* Wall::getTexture(UINT16 tile)
{
	std::string wallTex = std::to_string(tile);

	return GRAPHICS->getTexture(
		wallTex,
		"Assets/Textures/Walls/" + wallTex + ".png"
	);
}

void Wall::draw()
{
	WorldObjectCBuffer* cbuffer = (WorldObjectCBuffer*)shader->cBuffer;
	cbuffer->entityPos = position;
	shader->mapCBuffer();

	if (texture)
		model->draw(texture);
	else
		model->draw();
}

void Wall::update()
{

}
