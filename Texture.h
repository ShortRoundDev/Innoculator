#pragma once

#include <d3d11.h>

#include <wrl/client.h>

#include <string>

using namespace Microsoft::WRL;

class Texture
{
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string path);
	~Texture();

	bool status;

	void use();

	ComPtr<ID3D11Resource> textureResource;
	ComPtr<ID3D11Texture2D> texture2D;
	ComPtr<ID3D11ShaderResourceView> textureView;
};