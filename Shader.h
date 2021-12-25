#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <functional>
#include <wrl/client.h>

#include "GlobalCBuffer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Shader
{
public:
	Shader(
		ID3D11Device* device,
		std::wstring vertPath,
		std::wstring pixelPath,
		bool lerp,
		void* cBuffer,
		size_t cBufferSize
	);

	~Shader();

	bool status;

	void* cBuffer;
	size_t cBufferSize;

	ComPtr<ID3D11VertexShader> vertex;
	ComPtr<ID3D11PixelShader> pixel;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11BlendState> blendState;

	void mapCBuffer();
	void use();

private:
	void logShaderError(ID3D10Blob* shaderError, std::wstring path);
};
