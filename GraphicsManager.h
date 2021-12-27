#pragma once

#include "GlobalCBuffer.h"

#include "d3d11.h"
#include <wrl/client.h>
#include <DirectXMath.h>

#include <memory>
#include <map>
#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;

//forward declarations
class Shader;
class Texture;
class IModel;
class Camera;

#define GRAPHICS (GraphicsManager::instance)
#define DEVICE (GRAPHICS->device)
#define CONTEXT (GRAPHICS->deviceContext)

class GraphicsManager
{

public:
	static std::unique_ptr<GraphicsManager> instance;
	static void init();

	void postInit();

	GraphicsManager();
	~GraphicsManager();

	std::map<std::string, Shader*> shaders;
	std::map<std::string, IModel*> models;
	std::map<std::string, Texture*> textures;

	ComPtr<IDXGISwapChain> swapchain;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11RasterizerState> rasterState;

	XMMATRIX projectionMatrix;
	XMMATRIX ortho;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;

	ComPtr<ID3D11Buffer> matrixBuffer;
	GlobalCBuffer* cBuffer;


	int numerator;
	int denominator;

	void clear();
	void clearDepth();
	void swap();

	Camera* camera;

	bool status;

	void update();
	void updateGlobalCBuffer(bool useOrthogonal);

	Texture* getTexture(std::string name, std::string path);
	Texture* getTexture(std::string name);
	Texture* getTexture(std::string name, BYTE* data, size_t size);
	IModel* getModel(std::string name, std::string path);
private:
	bool initInfrastructure();
	bool initSwapchain();
	bool initBackBuffer();
	bool initDepthStencilBuffer();
	bool initRasterizer();
	bool initGlobalCBuffer();
	bool initShaders();
	bool initTextures();
	bool initModels();

	void inverseTranspose(_In_ XMMATRIX* world, _Out_ XMMATRIX* invWorld);
};