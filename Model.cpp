#include "Model.h"

#include "GraphicsManager.h"
#include "SystemManager.h"
#include "Texture.h"

#include "WaveFrontReader.h"

#include <codecvt>

Model::Model(ID3D11Device* device, ID3D11DeviceContext* context, std::string file) :
	vertexCount(0),
	indexCount(0),
	texture(NULL),
	status(true)
{
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

	init(device, context, (float*)v, reader.vertices.size() * sizeof(Vertex), i, reader.indices.size(), t);

	status = true;
}

Model::Model(std::string file) : Model(GRAPHICS->device.Get(), GRAPHICS->deviceContext.Get(), file)
{

}

void Model::init(ID3D11Device* device, ID3D11DeviceContext* context, float* mesh, size_t meshSize, UINT* indices, size_t indexCount, Texture* texture)
{
	Vertex* vertices = (Vertex*)mesh;
	vertexCount = (int)(meshSize / sizeof(Vertex));

	this->indexCount = (int)indexCount;

	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = (UINT)meshSize;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertexDesc, &vertexData, this->vertexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		status = false;
		LOG_ERROR(LOGGER, "Failed to create D3D buffer for model vertices!");
		return; // todo: error
	}

	D3D11_BUFFER_DESC indexDesc;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.ByteWidth = sizeof(ULONG) * (UINT)indexCount;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexDesc, &indexData, this->indexBuffer.GetAddressOf());
	if (FAILED(result))
	{
		status = false;
		LOG_ERROR(LOGGER, "Failed to create D3D buffer for model indices!");
		return; // todo: error
	}
	this->texture = texture;
}

Model::~Model()
{

}


void Model::draw()
{
	draw(texture);
}

void Model::draw(Texture* texture)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	texture->use();

	CONTEXT->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	CONTEXT->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->DrawIndexed(indexCount, 0, 0);
}
