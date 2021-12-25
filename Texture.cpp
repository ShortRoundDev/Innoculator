#include "Texture.h"

#include "WICTextureLoader.h"
#include "GraphicsManager.h"

#include <codecvt>

using namespace DirectX;

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, std::string path)
{

	std::wstring winPath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(path);
	HRESULT result = CreateWICTextureFromFile(device, winPath.c_str(), textureResource.GetAddressOf(), textureView.GetAddressOf());
	if (FAILED(result))
	{
		status = false;
		return;
	}

	result = textureResource->QueryInterface(IID_ID3D11Texture2D, (void**)texture2D.GetAddressOf());
	if (FAILED(result))
	{
		status = false;
		return;
	}

	status = true;
}

Texture::~Texture()
{

}

void Texture::use()
{
	if (textureView == NULL)
	{
		return;
	}
	if (CONTEXT == NULL)
	{
		return;
	}
	CONTEXT->PSSetShaderResources(0, 1, textureView.GetAddressOf());

}