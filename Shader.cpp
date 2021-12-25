#include "Shader.h"

#include "SystemManager.h"
#include "GraphicsManager.h"
#include "LocalShaderIncluder.h"

#include <d3dcompiler.h>

Shader::Shader(
	ID3D11Device* device,
	std::wstring vertPath,
	std::wstring pixelPath,
	bool lerp,
	void* cBuffer,
	size_t cBufferSize
):
	cBuffer(cBuffer),
	status(true),
	cBufferSize(cBufferSize + (16 - cBufferSize % 16)) // padding
{
	ComPtr<ID3D10Blob> vertexBuffer;
	ComPtr<ID3D10Blob> compileError;

	ID3DInclude* includer = new LocalShaderIncluder();

	//compile vertex
	HRESULT result = D3DCompileFromFile(
		vertPath.c_str(),
		NULL,
		includer,
		"Vertex",
		"vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		vertexBuffer.GetAddressOf(),
		compileError.GetAddressOf()
	);
	if (FAILED(result)) {
		if (compileError) {
			logShaderError(compileError.Get(), vertPath);
		}
		else {
			LOG_ERROR(LOGGER, "Missing vertex shader file!");
		}
		status = false;
		return;
	}

	ComPtr<ID3D10Blob> pixelBuffer;
	
	//compile pixel
	result = D3DCompileFromFile(
		pixelPath.c_str(),
		NULL,
		includer,
		"Pixel",
		"ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		pixelBuffer.GetAddressOf(),
		compileError.GetAddressOf()
	);
	if (FAILED(result)) {
		if (compileError) {
			logShaderError(compileError.Get(), pixelPath);
		}
		else {
			LOG_ERROR(LOGGER, "Missing pixel shader file!");
		}
		status = false;
		return;
	}

	//build

	result = device->CreateVertexShader(
		vertexBuffer->GetBufferPointer(),
		vertexBuffer->GetBufferSize(),
		NULL,
		vertex.GetAddressOf()
	);

	if (FAILED(result)) {
		LOG_ERROR(LOGGER, "Vertex Shader Link Error!");
		status = false;
		return;
	}

	result = device->CreatePixelShader(
		pixelBuffer->GetBufferPointer(),
		pixelBuffer->GetBufferSize(),
		NULL,
		pixel.GetAddressOf()
	);

	if (FAILED(result)) {
		LOG_ERROR(LOGGER, "Pixel Shader Link Error!");
		status = false;
		return;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	ZeroMemory(polygonLayout, sizeof(D3D11_INPUT_ELEMENT_DESC) * 3);
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	int numElements = _countof(polygonLayout);

	result = device->CreateInputLayout(
		polygonLayout,
		numElements,
		vertexBuffer->GetBufferPointer(),
		vertexBuffer->GetBufferSize(),
		inputLayout.GetAddressOf()
	);
	if (FAILED(result))
	{
		LOG_ERROR(LOGGER, "Shader input layout error!");
		status = false;
		return;
	}

	if (cBuffer != NULL)
	{
		D3D11_BUFFER_DESC matrixBufferDesc;
		ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = (UINT)this->cBufferSize;
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&matrixBufferDesc, NULL, this->matrixBuffer.GetAddressOf());
		if (FAILED(result))
		{
			LOG_ERROR(LOGGER, "Shader CBuffer creation error!");
			status = false;
			return;
		}
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = lerp ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	if (FAILED(result))
	{
		LOG_ERROR(LOGGER, "Shader sampler creation error!");
		status = false;
		return;
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
	if (FAILED(result)) {
		LOG_ERROR(LOGGER, "Shader blend state creation error!");
		status = false;
		return;
	}

	status = true;
}

Shader::~Shader()
{

}

void Shader::logShaderError(ID3D10Blob* shaderError, std::wstring path)
{
	char* compileErrors;
	ULONGLONG bufferSize;

	compileErrors = (char*)(shaderError->GetBufferPointer());
	bufferSize = shaderError->GetBufferSize();

	std::string errorMsg = std::string(compileErrors, bufferSize);
	std::string spath = std::string(path.begin(), path.end());

	LOG_ERROR(LOGGER, "Error compiling [{}]:\n{}", spath.c_str(), errorMsg.c_str());
}


void Shader::mapCBuffer()
{
	if (cBuffer == NULL) {
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;

	HRESULT result = CONTEXT->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(result))
	{
		LOG_ERROR(LOGGER, "Failed to lock CBuffer!");
		return;
	}

	GlobalCBuffer* localCBuffer = (GlobalCBuffer*)mapped.pData;

	CopyMemory(localCBuffer, cBuffer, cBufferSize);

	CONTEXT->Unmap(matrixBuffer.Get(), 0);

	// per-resource cbuffer lives in r1
	CONTEXT->VSSetConstantBuffers(1, 1, matrixBuffer.GetAddressOf());
	CONTEXT->PSSetConstantBuffers(1, 1, matrixBuffer.GetAddressOf());
}

void Shader::use()
{
	CONTEXT->IASetInputLayout(inputLayout.Get());
	CONTEXT->VSSetShader(vertex.Get(), NULL, 0);
	CONTEXT->PSSetShader(pixel.Get(), NULL, 0);
	CONTEXT->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	float factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	CONTEXT->OMSetBlendState(blendState.Get(), factor, 0xffffffff);
}