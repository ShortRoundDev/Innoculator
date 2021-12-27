#pragma once

#include "IModel.h"

#include <string>

class Texture;

class AnimatedModel : public IModel
{
public:
	AnimatedModel(std::string path);
	~AnimatedModel();

	virtual void draw();
	virtual void draw(Texture* texture);

};

