#include "GraphicsManager.h"
#include "SystemManager.h"

#include "Camera.h"
#include "Texture.h"
#include "IModel.h"
#include "WaveFrontModel.h"
#include "AnimatedModel.h"
#include "Shader.h"

// ===== CBUFFERS ===== //

#include "WorldObjectCBuffer.h"

// ==================== //

#include <math.h>

#pragma warning(push, 0)
#include "yaml-cpp/yaml.h"
#pragma warning(pop)

#define M_PI_4 (0.7853981633974f)

std::unique_ptr<GraphicsManager> GraphicsManager::instance;

void GraphicsManager::init()
{
	instance = std::make_unique<GraphicsManager>();
	instance->postInit();
}

GraphicsManager::GraphicsManager() :
	projectionMatrix(),
	worldMatrix(),
	orthoMatrix(),
	textures(),
	models(),
	camera(new Camera)
{
	if (!initInfrastructure()) {
		goto fail;
	}
	if (!initSwapchain()) {
		goto fail;
	}
	if (!initBackBuffer()) {
		goto fail;
	}
	if (!initDepthStencilBuffer()) {
		goto fail;
	}
	if (!initRasterizer()) {
		goto fail;
	}
	if (!initGlobalCBuffer()) {
		goto fail;
	}

	status = true;
	return;

fail:
	status = false;
	return;
}

void GraphicsManager::postInit()
{
	if (!initShaders()) {
		goto fail;
	}
	if (!initTextures()) {
		goto fail;
	}
	if (!initModels()) {
		goto fail;
	}

	status = true;
	return;

fail:
	status = false;
	return;
}

GraphicsManager::~GraphicsManager()
{

}

bool GraphicsManager::initInfrastructure()
{
	ComPtr<IDXGIFactory> factory = NULL;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
	{
		return false;
	}

	IDXGIAdapter* adapter = NULL;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	ComPtr<IDXGIOutput> output = NULL;
	if (FAILED(adapter->EnumOutputs(0, &output)))
	{
		return false;
	}

	//Count total
	UINT numModes = 0;
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
	{
		return false;
	}

	//Actually enumerate now
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
	{
		return false;
	}

	for (UINT i = 0; i < numModes; i++)
	{
		if ((displayModeList[i].Width == (UINT)SYSTEM->screenWidth) && (displayModeList[i].Height == (UINT)SYSTEM->screenHeight))
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	delete[] displayModeList;

	return true;
}

bool GraphicsManager::initSwapchain()
{
	DXGI_SWAP_CHAIN_DESC swapchainDesc = { };
	ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapchainDesc.BufferCount = 1;
	swapchainDesc.BufferDesc.Width = SYSTEM->screenWidth;
	swapchainDesc.BufferDesc.Height = SYSTEM->screenHeight;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = SYSTEM->vsync ? numerator : 0;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = SYSTEM->vsync ? denominator : 1;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.OutputWindow = SYSTEM->hwnd;
	swapchainDesc.SampleDesc.Count = 4;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.Windowed = !SYSTEM->fullScreen;
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	if (
		FAILED(
			D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				0,
				&featureLevel,
				1,
				D3D11_SDK_VERSION,
				&swapchainDesc,
				&swapchain,
				&device,
				NULL,
				&deviceContext
			)
		)
		)
	{
		return false;
	}
	return true;
}

bool GraphicsManager::initBackBuffer()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	if (FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(backBuffer.GetAddressOf()))))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	if (FAILED(device->CreateRenderTargetView(backBuffer.Get(), &rtvDesc, renderTargetView.GetAddressOf())))
	{
		return false;
	}

	return true;
}

bool GraphicsManager::initDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthBufferDesc = { };
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthBufferDesc.Width = SYSTEM->screenWidth;
	depthBufferDesc.Height = SYSTEM->screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 4;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf())))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { };
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf())))
	{
		return false;
	}

	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = { };
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf())))
	{
		return false;
	}

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	return true;
}

bool GraphicsManager::initRasterizer()
{
	D3D11_RASTERIZER_DESC rasterDesc = { };
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(device->CreateRasterizerState(&rasterDesc, rasterState.GetAddressOf())))
	{
		return true;
	}

	deviceContext->RSSetState(rasterState.Get());

	D3D11_VIEWPORT viewport = { };
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = (float)SYSTEM->screenWidth;
	viewport.Height = (float)SYSTEM->screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);
	float fov = (float)M_PI_4;
	float aspect = (float)SYSTEM->screenWidth / (float)SYSTEM->screenHeight;

	projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 1000.0f);
	ortho = XMMatrixOrthographicLH((float)SYSTEM->screenWidth, (float)SYSTEM->screenHeight, 0.1f, 1000.0f);
	worldMatrix = XMMatrixIdentity();
	orthoMatrix = XMMatrixOrthographicLH((float)SYSTEM->screenWidth, (float)SYSTEM->screenHeight, 0.1f, 1000.f);

	return true;
}

bool GraphicsManager::initGlobalCBuffer()
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(GlobalCBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, this->matrixBuffer.GetAddressOf());
	if (FAILED(result))
	{		
		return false;
	}

	cBuffer = new GlobalCBuffer;

	return true;
}

bool GraphicsManager::initShaders()
{
	/*auto status = RegisterShaders(device.Get(), &shaders);
	if (status != )
	{
		return GraphicsManagerStatus::D3D_SHADER_INIT_ERROR;
	}

	return GraphicsManagerStatus::OK;*/

	Shader* worldObjectShader = new Shader(
		device.Get(),
		L"WorldObjectVertex.hlsl",
		L"WorldObjectPixel.hlsl",
		true,
		new WorldObjectCBuffer,
		sizeof(WorldObjectCBuffer)
	);
	if (worldObjectShader->status) {
		shaders["WorldObject"] = worldObjectShader;
	}
	else {
		return false;
	}
	return true;
}

bool GraphicsManager::initModels()
{	
	YAML::Node config = YAML::LoadFile("models.yaml");
	if (!config["Models"]) {
		return false;
	}

	auto modelList = config["Models"];
	auto size = modelList.size();
	for (auto member : modelList) {
		getModel(member.first.as<std::string>(), member.second.as<std::string>());
	}

	return true;
}

bool GraphicsManager::initTextures()
{
	YAML::Node config = YAML::LoadFile("textures.yaml");
	if (!config["Textures"]) {
		return false;
	}

	auto textureList = config["Textures"];
	auto size = textureList.size();
	for (auto member : textureList) {
		getTexture(member.first.as<std::string>(), member.second.as<std::string>());
	}

	return true;
}

void GraphicsManager::update()
{
	if (camera)
		camera->update();
}

void GraphicsManager::clear()
{
	float color[4] = { 0, 0.5f, 0, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GraphicsManager::clearDepth()
{
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GraphicsManager::swap()
{
	if (SYSTEM->vsync)
	{
		swapchain->Present(1, 0);
	}
	else
	{
		swapchain->Present(0, 0);
	}
}

Texture* GraphicsManager::getTexture(std::string name, std::string path)
{
	auto found = textures.find(name);
	if (found == textures.end()) {
		if (path.empty()) {
			return nullptr;
		}
		Texture* loadTexture = new Texture(device.Get(), deviceContext.Get(), path);
		if (loadTexture->status == false) {
			delete loadTexture;
			return nullptr;
		}

		textures[name] = loadTexture;
		return loadTexture;
	}

	return found->second;
}

Texture* GraphicsManager::getTexture(std::string name)
{
	return getTexture(name, "");
}
Texture* GraphicsManager::getTexture(std::string name, BYTE* data, size_t size)
{
	auto found = textures.find(name);
	if (found == textures.end()) {
		Texture* loadTexture = new Texture(device.Get(), deviceContext.Get(), data, size);
		if (loadTexture->status == false) {
			delete loadTexture;
			return nullptr;
		}

		textures[name] = loadTexture;
		return loadTexture;
	}

	return found->second;

}


IModel* GraphicsManager::getModel(std::string name, std::string path)
{
	auto found = models.find(name);
	if (found == models.end()) {
		const char* ptr = (path.c_str()) + path.size() - 4;
		IModel* loadModel = nullptr;
		if (!strcmp(ptr, ".obj")) {
			loadModel = new WaveFrontModel(device.Get(), deviceContext.Get(), path);
		}
		else if(!strcmp(ptr, ".glb")) {
			loadModel = new AnimatedModel(path);
		}

		if (!loadModel) {
			return nullptr;
		}

		if (loadModel->status == false) {
			delete loadModel;
			return nullptr;
		}

		models[name] = loadModel;
		return loadModel;
	}

	return found->second;
}


void GraphicsManager::updateGlobalCBuffer(bool useOrthogonal)
{
	cBuffer->world = worldMatrix;
	cBuffer->projection = projectionMatrix;
	if (useOrthogonal) {
		cBuffer->view = camera->orthoViewMatrix;
	}
	else {
		cBuffer->view = camera->viewMatrix;
	}

	XMMATRIX _world = XMMatrixTranspose(cBuffer->world);
	XMMATRIX _view = XMMatrixTranspose(cBuffer->view);
	XMMATRIX _projection;
	if (useOrthogonal)
	{
		_projection = XMMatrixTranspose(ortho);
	}
	else
	{
		_projection = XMMatrixTranspose(cBuffer->projection);
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	// Lock
	HRESULT result = deviceContext->Map(matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(result))
	{
		LOG_ERROR(LOGGER, "Failed to lock global matrix buffer! {}", result);
		return;
	}

	GlobalCBuffer* localCBuffer = (GlobalCBuffer*)mapped.pData;
	CopyMemory(localCBuffer, cBuffer, sizeof(GlobalCBuffer));
	localCBuffer->cameraPos = {
		camera->position.x,
		camera->position.y,
		camera->position.z,
		0
	}; // cbuffer camera is xmfloat4 for alignment.

	localCBuffer->world = _world;
	localCBuffer->view = _view;
	localCBuffer->projection = _projection;

	inverseTranspose(&_world, &localCBuffer->invWorld);

	//Unlock
	deviceContext->Unmap(matrixBuffer.Get(), 0);

	// Set at slot 0 per-frame
	deviceContext->VSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, matrixBuffer.GetAddressOf());
}

void GraphicsManager::inverseTranspose(_In_ XMMATRIX* world, _Out_ XMMATRIX* invWorld)
{
	CopyMemory(invWorld, world, sizeof(XMMATRIX));
	invWorld->r[3] = XMVectorSet(0, 0, 0, 1.0f);
	XMVECTOR determinant = XMMatrixDeterminant(*invWorld);
	*invWorld = XMMatrixTranspose(XMMatrixInverse(&determinant, *invWorld));
}
