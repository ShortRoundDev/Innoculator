#pragma once

#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>
#include "IModel.h"

#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;

//forward declarations
class Texture;

class WaveFrontModel : public IModel
{
public:
	WaveFrontModel(ID3D11Device* device, ID3D11DeviceContext* context, std::string file);
	WaveFrontModel(std::string file);
	~WaveFrontModel();

	void draw();
	void draw(Texture* texture);
};

