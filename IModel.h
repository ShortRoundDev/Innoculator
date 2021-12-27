#pragma once

#include "base.h"
#include <d3d11.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class Texture;

class IModel
{
public:
	virtual void draw();
	virtual void draw(Texture* texture);
	virtual void update();

	Texture* texture;
	ComPtr<ID3D11Buffer> vertexBuffer;
	int vertexCount;

	ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount;

	void init(ID3D11Device* device, ID3D11DeviceContext* context, float* mesh, size_t meshSize, UINT* indices, size_t indexSize, Texture* texture);

	bool status;
};

