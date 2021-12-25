#pragma once

#include <DirectXMath.h>
#include <wrl/client.h>
#include <d3d11.h>

#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texture;
};

//forward declarations
class Texture;

class Model
{
public:
	Model(ID3D11Device* device, ID3D11DeviceContext* context, std::string file);
	Model(std::string file);
	~Model();

	bool status;

	Texture* texture;
	ComPtr<ID3D11Buffer> vertexBuffer;
	int vertexCount;

	ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount;

	void draw();
	void draw(Texture* texture);
private:
	void init(ID3D11Device* device, ID3D11DeviceContext* context, float* mesh, size_t meshSize, UINT* indices, size_t indexSize, Texture* texture);
};

