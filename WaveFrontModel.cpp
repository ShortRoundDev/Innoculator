#include "WaveFrontModel.h"

#include "GraphicsManager.h"
#include "SystemManager.h"
#include "Texture.h"
#include "ModelVertex.h"

#include "WaveFrontReader.h"

#include <codecvt>

WaveFrontModel::WaveFrontModel(ID3D11Device* device, ID3D11DeviceContext* context, std::string file){
	WaveFrontReader<UINT> reader;

	std::wstring winPath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(file);
	HRESULT hr = reader.Load(winPath.c_str());
	if (FAILED(hr)) {
		status = false;
		return;
	}

	std::wstring textureNameW = std::wstring(reader.materials[1].strTexture);
	std::string textureName = std::string(textureNameW.begin(), textureNameW.end());

	auto v = &(reader.vertices[0]);
	UINT* i = &(reader.indices[0]);

	Texture* t = GRAPHICS->getTexture(textureName, textureName);

	init(device, context, (float*)v, reader.vertices.size() * sizeof(ModelVertex), i, reader.indices.size(), t);

	status = true;
}

WaveFrontModel::WaveFrontModel(std::string file) : WaveFrontModel(GRAPHICS->device.Get(), GRAPHICS->deviceContext.Get(), file)
{

}

WaveFrontModel::~WaveFrontModel()
{

}


void WaveFrontModel::draw()
{
	draw(texture);
}

void WaveFrontModel::draw(Texture* texture)
{
	UINT stride = sizeof(ModelVertex);
	UINT offset = 0;
	texture->use();

	CONTEXT->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	CONTEXT->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->DrawIndexed(indexCount, 0, 0);
}
