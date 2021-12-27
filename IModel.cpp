#include "IModel.h"

#include "ModelVertex.h"
#include "SystemManager.h"

void IModel::draw()
{

}

void IModel::draw(Texture* texture)
{

}

void IModel::update()
{

}

void IModel::init(ID3D11Device* device, ID3D11DeviceContext* context, float* mesh, size_t meshSize, UINT* indices, size_t indexCount, Texture* texture)
{
	ModelVertex* vertices = (ModelVertex*)mesh;
	vertexCount = (int)(meshSize / sizeof(ModelVertex));

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
		LOG_ERROR(LOGGER, "Failed to create D3D buffer for WaveFrontModel vertices!");
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
		LOG_ERROR(LOGGER, "Failed to create D3D buffer for WaveFrontModel indices!");
		return; // todo: error
	}
	this->texture = texture;
}