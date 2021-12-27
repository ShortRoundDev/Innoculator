#include "AnimatedModel.h"

#include "base.h"
#include "Texture.h"
#include "GraphicsManager.h"
#include "ModelVertex.h"

#include <algorithm>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>
#include "FileStreamReader.h"

#include <DirectXMath.h>

using namespace Microsoft;
using namespace DirectX;

AnimatedModel::AnimatedModel(std::string path)
{
	auto streamReader = std::make_unique<FileStreamReader>();
	auto glbStream = streamReader->GetInputStream(path);
	auto reader = std::make_unique<glTF::GLBResourceReader>(std::move(streamReader), std::move(glbStream));
	auto manifest = reader->GetJson();

	auto document = glTF::Deserialize(manifest);
	
	for (auto image : document.images.Elements()) {
		auto data = reader->ReadBinaryData(document, image);
		texture = GRAPHICS->getTexture(image.name, data.data(), data.size());
	}

	for (auto mesh : document.meshes.Elements()) {
		std::string accessorId;

		for(auto primitive : mesh.primitives){

			auto indexAccessor = document.accessors.Get(primitive.indicesAccessorId);
			auto indexData = reader->ReadBinaryData<UINT16>(document, indexAccessor);

			UINT* indices = (UINT*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, indexData.size() * sizeof(UINT));
			indexCount = indexData.size();

			for (int i = 0; i < indexData.size(); i++) {
				indices[i] = (UINT)indexData[i];
			}

			ModelVertex* vertices;
			size_t meshSize;

			if (primitive.TryGetAttributeAccessorId(glTF::ACCESSOR_POSITION, accessorId)) {
				auto accessor = document.accessors.Get(accessorId);
				auto vertexData = reader->ReadBinaryData<float>(document, accessor);

				meshSize = (vertexData.size() / 3) * sizeof(ModelVertex);

				vertices = (ModelVertex*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, meshSize);
				if (!vertices) {
					continue;
				}

				XMFLOAT3* posData = (XMFLOAT3*)vertexData.data();
				for (int i = 0; i < vertexData.size() / 3; i++)
				{
					vertices[i].position = posData[i];
				}
			}
			else {
				continue;
			}

			if (primitive.TryGetAttributeAccessorId(glTF::ACCESSOR_NORMAL, accessorId)) {
				auto accessor = document.accessors.Get(accessorId);
				auto normalData = reader->ReadBinaryData<float>(document, accessor);

				XMFLOAT3* normals = (XMFLOAT3*)normalData.data();
				for (int i = 0; i < normalData.size() / 3; i++) {
					vertices[i].normal = normals[i];
				}
			}

			if (primitive.TryGetAttributeAccessorId(glTF::ACCESSOR_TEXCOORD_0, accessorId)) {
				auto accessor = document.accessors.Get(accessorId);
				auto texData = reader->ReadBinaryData<float>(document, accessor);

				XMFLOAT2* tex = (XMFLOAT2*)texData.data();
				for (int i = 0; i < texData.size() / 2; i++) {
					vertices[i].tex = tex[i];
				}
			}

			init(DEVICE.Get(), CONTEXT.Get(), (float*)vertices, meshSize, indices, indexCount, texture);
		}
	}
	status = true;
}

AnimatedModel::~AnimatedModel()
{

}

void AnimatedModel::draw()
{
	draw(texture);
}

void AnimatedModel::draw(Texture* texture)
{
	UINT stride = sizeof(ModelVertex);
	UINT offset = 0;
	texture->use();

	CONTEXT->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	CONTEXT->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->DrawIndexed(indexCount, 0, 0);
}
