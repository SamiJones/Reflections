/*The code / approaches used for:
	- Setup of render target cameras in the initialiseSceneResources() function
	- Creation of the cube map texture in the renderScene() function

	is based on the code of Frank Luna in "Introduction to 3D Game Programming with DirectX11".
	See "http://aranna.altervista.org/data2/3d_game_programming_with_DirectX11.pdf" Chapter 17
*/

#include <stdafx.h>
#include <string.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <Scene.h>
#include <DirectXMath.h>
#include <DXSystem.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <CGDClock.h>
#include <Model.h>
#include <LookAtCamera.h>
#include <FirstPersonCamera.h>
#include <Material.h>
#include <Effect.h>
#include <Texture.h>
#include <VertexStructures.h>
#include <GPUParticles.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

// Load the Compiled Shader Object (CSO) file 'filename' and return the bytecode in the blob object **bytecode.  This is used to create shader interfaces that require class linkage interfaces.
// Taken from DXShaderFactory by Paul Angel. This function has been included here for clarity.
uint32_t DXLoadCSO(const char *filename, char **bytecode)
{

	ifstream	*fp = nullptr;
	//char		*memBlock = nullptr;
	//bytecode = nullptr;
	uint32_t shaderBytes = -1;
	cout << "loading shader" << endl;

	try
	{
		// Validate parameters
		if (!filename)
			throw exception("loadCSO: Invalid parameters");

		// Open file
		fp = new ifstream(filename, ios::in | ios::binary);

		if (!fp->is_open())
			throw exception("loadCSO: Cannot open file");

		// Get file size
		fp->seekg(0, ios::end);
		shaderBytes = (uint32_t)fp->tellg();

		// Create blob object to store bytecode (exceptions propagate up if any occur)
		//memBlock = new DXBlob(size);
		cout << "allocating shader memory bytes = " << shaderBytes << endl;
		*bytecode = (char*)malloc(shaderBytes);
		// Read binary data into blob object
		fp->seekg(0, ios::beg);
		fp->read(*bytecode, shaderBytes);


		// Close file and release local resources
		fp->close();
		delete fp;

		// Return DXBlob - ownership implicity passed to caller
		//*bytecode = memBlock;
		cout << "Done: shader memory bytes = " << shaderBytes << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;

		// Cleanup local resources
		if (fp) {

			if (fp->is_open())
				fp->close();

			delete fp;
		}

		if (bytecode)
			delete bytecode;

		// Re-throw exception
		throw;
	}
	return shaderBytes;
}

//
// Private interface implementation
//

// Private constructor
Scene::Scene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {

	for (int i = 0; i < 6; i++)
		renderTargetCameras[i] = nullptr;

	try
	{
		// 1. Register window class for main DirectX window
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = wndClassName;
		wcex.hIconSm = NULL;

		if (!RegisterClassEx(&wcex))
			throw exception("Cannot register window class for Scene HWND");


		// 2. Store instance handle in our global variable
		hInst = hInstance;


		// 3. Setup window rect and resize according to set styles
		RECT		windowRect;

		windowRect.left = 0;
		windowRect.right = _width;
		windowRect.top = 0;
		windowRect.bottom = _height;

		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		// 4. Create and validate the main window handle
		wndHandle = CreateWindowEx(dwExStyle, wndClassName, wndTitle, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 500, 500, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInst, this);

		if (!wndHandle)
			throw exception("Cannot create main window handle");

		ShowWindow(wndHandle, nCmdShow);
		UpdateWindow(wndHandle);
		SetFocus(wndHandle);


		// 5. Initialise render pipeline model (simply sets up an internal std::vector of pipeline objects)


		// 6. Create DirectX host environment (associated with main application wnd)
		dx = DXSystem::CreateDirectXSystem(wndHandle);

		if (!dx)
			throw exception("Cannot create Direct3D device and context model");

		// 7. Setup application-specific objects
		HRESULT hr = initialiseSceneResources();

		if (!SUCCEEDED(hr))
			throw exception("Cannot initalise scene resources");


		// 8. Create main clock / FPS timer (do this last with deferred start of 3 seconds so min FPS / SPF are not skewed by start-up events firing and taking CPU cycles).
		mainClock = CGDClock::CreateClock(string("mainClock"), 3.0f);

		if (!mainClock)
			throw exception("Cannot create main clock / timer");

	}
	catch (exception &e)
	{
		cout << e.what() << endl;

		// Re-throw exception
		throw;
	}

}

// Return TRUE if the window is in a minimised state, FALSE otherwise
BOOL Scene::isMinimised() {

	WINDOWPLACEMENT				wp;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);

	return (GetWindowPlacement(wndHandle, &wp) != 0 && wp.showCmd == SW_SHOWMINIMIZED);
}

//
// Public interface implementation
//

// Factory method to create the main Scene instance (singleton)
Scene* Scene::CreateScene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc) {

	static bool _scene_created = false;

	Scene *dxScene = nullptr;

	if (!_scene_created) {

		dxScene = new Scene(_width, _height, wndClassName, wndTitle, nCmdShow, hInstance, WndProc);

		if (dxScene)
			_scene_created = true;
	}

	return dxScene;
}

// Destructor
Scene::~Scene() {


	//free local resources

	if (cBufferExtSrc)
		_aligned_free(cBufferExtSrc);

	if (mainCamera)
		delete(mainCamera);

	for (int i = 0; i < 6; i++)
	{
		if (renderTargetCameras[i])
			delete(renderTargetCameras[i]);
	}

	if (brickTexture)
		delete(brickTexture);

	if (perPixelLightingEffect)
		delete(perPixelLightingEffect);

	//Clean Up- release local interfaces

	if (mainClock)
		mainClock->release();

	if (cBufferBridge)
		cBufferBridge->Release();

	if (cBufferTowerA)
		cBufferTowerA->Release();

	if (cBufferKnight)
		cBufferKnight->Release();

	if(cBufferWalls)
		cBufferWalls->Release();

	if (cBufferSkyBox)
		cBufferSkyBox->Release();

	if (cBufferSphere)
		cBufferSphere->Release();

	if (bridge)
		bridge->release();

	if (towerA)
		towerA->release();

	if (walls)
		walls->release();

	if (knight)
		knight->release();

	if (box)
		box->release();

	if (sphere)
		sphere->release();

	if (dx) {

		dx->release();
		dx = nullptr;
	}

	if (wndHandle)
		DestroyWindow(wndHandle);
}

// Decouple the encapsulated HWND and call DestoryWindow on the HWND
void Scene::destoryWindow() {

	if (wndHandle != NULL) {

		HWND hWnd = wndHandle;

		wndHandle = NULL;
		DestroyWindow(hWnd);
	}
}

// Resize swap chain buffers and update pipeline viewport configurations in response to a window resize event
HRESULT Scene::resizeResources() {

	if (dx && !isMinimised()) {

		// Only process resize if the DXSystem *dx exists (on initial resize window creation this will not be the case so this branch is ignored)
		HRESULT hr = dx->resizeSwapChainBuffers(wndHandle);
		rebuildViewport(mainCamera);
		RECT clientRect;
		GetClientRect(wndHandle, &clientRect);

		renderScene();
	}

	return S_OK;
}

// Helper function to call updateScene followed by renderScene
HRESULT Scene::updateAndRenderScene() {
	ID3D11DeviceContext *context = dx->getDeviceContext();
	HRESULT hr = updateScene(context, mainCamera);

	if (SUCCEEDED(hr))
		hr = renderScene();

	return hr;
}

// Clock handling methods
void Scene::startClock() {

	mainClock->start();
}
void Scene::stopClock() {

	mainClock->stop();
}

void Scene::reportTimingData() {

	cout << "Actual time elapsed = " << mainClock->actualTimeElapsed() << endl;
	cout << "Game time elapsed = " << mainClock->gameTimeElapsed() << endl << endl;
	mainClock->reportTimingData();
}

//
// Event handling methods
//
// Process mouse move with the left button held down
void Scene::handleMouseLDrag(const POINT &disp) {
	//LookAtCamera
	//mainCamera->rotateElevation((float)-disp.y * 0.01f);
	//mainCamera->rotateOnYAxis((float)-disp.x * 0.01f);

	//FirstPersonCamera
	mainCamera->elevate((float)-disp.y * 0.01f);
	mainCamera->turn((float)-disp.x * 0.01f);
}

// Process mouse wheel movement
void Scene::handleMouseWheel(const short zDelta) {

	//LookAtCamera
	/*if (zDelta < 0)
		mainCamera->zoomCamera(1.2f);
		else if (zDelta>0)
		mainCamera->zoomCamera(0.9f);*/

	//FirstPersonCamera
	mainCamera->move(zDelta*0.01);
}

// Process key down event.  keyCode indicates the key pressed while extKeyFlags indicates the extended key status at the time of the key down event (see http://msdn.microsoft.com/en-gb/library/windows/desktop/ms646280%28v=vs.85%29.aspx).
void Scene::handleKeyDown(const WPARAM keyCode, const LPARAM extKeyFlags) {
	
	float x = 0, y = 0, z = 0;

	//move reflective sphere (and render target cameras) back to origin
	if (keyCode == VK_SPACE)
	{
		sphereTranslationMatrix = XMMatrixIdentity();
	
		for (int i = 0; i < 6; i++)
			renderTargetCameras[i]->setPos(XMVectorSet(0, 0, 0, 0));

		return;
	}
	//move reflective sphere up (+y)
	else if (keyCode == 0x57) //0x57 = "W"
		y += 0.3;
	//move reflective sphere left (-x)
	else if (keyCode == 0x41) //0x41 = "A"
		x -= 0.3;
	//move reflective sphere down (-y)
	else if (keyCode == 0x53) //0x53 = "S"
		y -= 0.3;
	//move reflective sphere right (+x)
	else if (keyCode == 0x44) //0x44 = "D"
		x += 0.3;
	//move reflective sphere forwards (+z)
	else if (keyCode == 0x51) //0x51 = "Q"
		z += 0.3;
	//move reflective sphere backwards (-z)
	else if (keyCode == 0x45) //0x45 = "E"
		z -= 0.3;

	//combine the new translation matrix with the current sphereTranslationMatrix
	sphereTranslationMatrix *= XMMatrixTranslation(x, y, z);

	//translate render target cameras along with the sphere so that the positions of reflected objects update correctly as the sphere moves
	for (int i = 0; i < 6; i++)
		renderTargetCameras[i]->setPos(renderTargetCameras[i]->getPos() + XMVectorSet(x, y, z, 0));
}

// Process key up event.  keyCode indicates the key released while extKeyFlags indicates the extended key status at the time of the key up event (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms646281%28v=vs.85%29.aspx).
void Scene::handleKeyUp(const WPARAM keyCode, const LPARAM extKeyFlags) {

	// Add key up handler here...
}

//
// Methods to handle initialisation, update and rendering of the scene
//


//a second function has been added to 
HRESULT Scene::rebuildViewport(Camera *camera){
	// Binds the render target view and depth/stencil view to the pipeline.
	// Sets up viewport for the main window (wndHandle) 
	// Called at initialisation or in response to window resize

	ID3D11DeviceContext *context = dx->getDeviceContext();

	if (!context)
		return E_FAIL;

	// Bind the render target view and depth/stencil view to the pipeline.
	ID3D11RenderTargetView* renderTargetView = dx->getBackBufferRTV();
	context->OMSetRenderTargets(1, &renderTargetView, dx->getDepthStencil());
	// Setup viewport for the main window (wndHandle)
	RECT clientRect;
	GetClientRect(wndHandle, &clientRect);

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(clientRect.right - clientRect.left);
	viewport.Height = static_cast<FLOAT>(clientRect.bottom - clientRect.top);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//Set Viewport
	context->RSSetViewports(1, &viewport);

	// Compute the projection matrix.

	camera->setProjMatrix(XMMatrixPerspectiveFovLH(0.25f*3.14, viewport.Width / viewport.Height, 1.0f, 1000.0f));

	return S_OK;
}
HRESULT Scene::rebuildRenderTargetViewport(Camera *camera) {
	// Binds the render target view and depth/stencil view to the pipeline.
	// Sets up viewport for the main window (wndHandle) 
	// Called at initialisation or in response to window resize

	ID3D11DeviceContext *context = dx->getDeviceContext();

	if (!context)
		return E_FAIL;

	// Bind the render target view and depth/stencil view to the pipeline.
	ID3D11RenderTargetView* renderTargetView = dx->getBackBufferRTV();
	context->OMSetRenderTargets(1, &renderTargetView, dx->getDepthStencil());
	// Setup viewport for the main window (wndHandle)
	RECT clientRect;
	GetClientRect(wndHandle, &clientRect);

	renderTargetViewport.TopLeftX = 0;
	renderTargetViewport.TopLeftY = 0;
	renderTargetViewport.Width = static_cast<FLOAT>(CUBEMAP_SIZE);
	renderTargetViewport.Height = static_cast<FLOAT>(CUBEMAP_SIZE);
	renderTargetViewport.MinDepth = 0.0f;
	renderTargetViewport.MaxDepth = 1.0f;
	//Set Viewport
	context->RSSetViewports(1, &renderTargetViewport);

	// Compute the projection matrix.

	//First parameter for projection matrix has been set to 0.5*PI (90 degrees).
	//This widens the field of view of each camera so that each face of the cube map reflection fits seamlessly with adjacent faces
	//It also removes the "blind spots" between cube map faces from the reflection
	camera->setProjMatrix(XMMatrixPerspectiveFovLH(0.5f*3.14, renderTargetViewport.Width / renderTargetViewport.Height, 1.0f, 1000.0f));

	return S_OK;
}

HRESULT Scene::LoadShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader){

	char *PSBytecodeLocal;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = DXLoadCSO(filename, &PSBytecodeLocal);

	PSBytecode = &PSBytecodeLocal;
	cout << "Done: PShader memory bytes = " << shaderBytes << endl;
	// Create shader object
	HRESULT hr = device->CreatePixelShader(PSBytecodeLocal, shaderBytes, NULL, pixelShader);

	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create PixelShader interface");
	return hr;
}

uint32_t Scene::LoadShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader){

	char *VSBytecodeLocal;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = DXLoadCSO(filename, &VSBytecodeLocal);

	cout << "Done: VShader memory bytes = " << shaderBytes << endl;

	*VSBytecode = VSBytecodeLocal;
	cout << "Done: VShader writting = " << shaderBytes << endl;
	HRESULT hr = device->CreateVertexShader(VSBytecodeLocal, shaderBytes, NULL, vertexShader);
	cout << "Done: VShader return = " << hr << endl;
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create VertexShader interface");
	return shaderBytes;
}

// Main resource setup for the application.  These are setup around a given Direct3D device.
HRESULT Scene::initialiseSceneResources() {
	//ID3D11DeviceContext *context = dx->getDeviceContext();
	ID3D11Device *device = dx->getDevice();
	if (!device)
		return E_FAIL;

	//
	// Setup main pipeline objects
	//

	// Setup objects for fixed function pipeline stages
	// Rasterizer Stage
	// Bind the render target view and depth/stencil view to the pipeline
	// and sets up viewport for the main window (wndHandle) 

	// Create main camera
	mainCamera = new FirstPersonCamera();
	mainCamera->setPos(XMVectorSet(25, 2, -14.5, 1));

	rebuildViewport(mainCamera);

	//
	//Setup of render target cameras
	//

	//Vectors for render target camera initial position, up direction ("up" relative to camera), and direction each camera is facing towards
	XMVECTOR pos = XMVectorSet(0, 0, 0, 1);
	
	XMVECTOR upDirections[6] = {
		XMVectorSet(0, 1, 0, 0), //+x
		XMVectorSet(0, 1, 0, 0), //-x
		XMVectorSet(0, 0, -1, 0), //+y
		XMVectorSet(0, 0, 1, 0), //-y
		XMVectorSet(0, 1, 0, 0), //+z
		XMVectorSet(0, 1, 0, 0), //-z
	};

	//directions each camera will face
	XMVECTOR faceDirections[6] = {
		XMVectorSet(1, 0, 0, 1),  //+x
		XMVectorSet(-1, 0, 0, 1), //-x
		XMVectorSet(0, 1, 0, 1), //+y    
		XMVectorSet(0, -1, 0, 1),  //-y
		XMVectorSet(0, 0, 1, 1),  //+z
		XMVectorSet(0, 0, -1, 1), //-z
	};

	//Initialise each of the render target cameras at the origin, facing in the appropriate direction
	for (int i = 0; i < 6; i++)
		renderTargetCameras[i] = new FirstPersonCamera(pos, upDirections[i], faceDirections[i]);

	ID3D11DeviceContext *context = dx->getDeviceContext();
	if (!context)
		return E_FAIL;

	//Tutorial 04 task 1 create render target texture

	D3D11_TEXTURE2D_DESC texDesc;
	// fill out texture descrition
	texDesc.Width = CUBEMAP_SIZE;
	texDesc.Height = CUBEMAP_SIZE;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	//texDesc.SampleDesc.Count = 8; // Multi-sample properties much match the above DXGI_SWAP_CHAIN_DESC structure
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	//Create Texture
	ID3D11Texture2D* renderTargetTexture = nullptr;
	HRESULT hr = device->CreateTexture2D(&texDesc, 0, &renderTargetTexture);

	//Create render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY; //was "...TEXTURE2DMS"
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;

		hr = device->CreateRenderTargetView(renderTargetTexture, &rtvDesc, &mDynamicCubeMapRTV[i]);
	}
 
	// Create the 6-face render target view
	// Used to create RTV for attempted geometry shader optimisation of cube map creation
	D3D11_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = texDesc.Format;
	DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	DescRT.Texture2DArray.FirstArraySlice = 0;
	DescRT.Texture2DArray.ArraySize = 6;
	DescRT.Texture2DArray.MipSlice = 0;

	hr = device->CreateRenderTargetView(renderTargetTexture, &DescRT, &mDynamicCubeMapRTV_SinglePass);
	//

	//Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	hr = device->CreateShaderResourceView(renderTargetTexture, &srvDesc, &mDynamicCubeMapSRV);

	// View saves reference.
	renderTargetTexture->Release();

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CUBEMAP_SIZE;
	depthTexDesc.Height = CUBEMAP_SIZE;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	hr = device->CreateTexture2D(&depthTexDesc, 0, &depthTex);
	// Create the depth stencil view for the entire buffer.
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &mDynamicCubeMapDSV);

	// Create the depth stencil view for the entire cube
	// Used to create DSV for attempted geometry shader optimisation of cube map creation
	D3D11_DEPTH_STENCIL_VIEW_DESC DescDS;
	DescDS.Format = depthTexDesc.Format;
	DescDS.Flags = 0;
	DescDS.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DescDS.Texture2DArray.FirstArraySlice = 0;
	DescDS.Texture2DArray.ArraySize = 1;
	DescDS.Texture2DArray.MipSlice = 0;

	hr = device->CreateDepthStencilView(depthTex, &DescDS, &mDynamicCubeMapDSV_SinglePass);
	//

	//set up viewport
	renderTargetViewport.TopLeftX = 0.0f;
	renderTargetViewport.TopLeftY = 0.0f;
	renderTargetViewport.Width = (float)CUBEMAP_SIZE;
	renderTargetViewport.Height = (float)CUBEMAP_SIZE;
	renderTargetViewport.MinDepth = 0.0f;
	renderTargetViewport.MaxDepth = 1.0f;

	// Setup objects for the programmable (shader) stages of the pipeline

	perPixelLightingEffect = new Effect(device, "Shaders\\cso\\per_pixel_lighting_vs.cso", "Shaders\\cso\\per_pixel_lighting_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	skyBoxEffect = new Effect(device, "Shaders\\cso\\sky_box_vs.cso", "Shaders\\cso\\sky_box_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));
	basicEffect = new Effect(device, "Shaders\\cso\\basic_texture_vs.cso", "Shaders\\cso\\basic_texture_ps.cso", basicVertexDesc, ARRAYSIZE(basicVertexDesc));
	fireEffect = new Effect(device, "Shaders\\cso\\fire_vs.cso", "Shaders\\cso\\fire_ps.cso", "Shaders\\cso\\fire_gs.cso", particleVertexDesc, ARRAYSIZE(particleVertexDesc));

	//Used for standard implementation of reflection
	refMapEffect = new Effect(device, "Shaders\\cso\\reflection_map_vs.cso", "Shaders\\cso\\reflection_map_ps.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));

	//Used for attempted optimisation of reflection cube map creation using geometry shader.
	//Not currently used.
	//refMapEffect = new Effect(device, "Shaders\\cso\\reflection_map_vs.cso", "Shaders\\cso\\reflection_map_ps.cso", "Shaders\\cso\\reflection_map_gs.cso", extVertexDesc, ARRAYSIZE(extVertexDesc));

	// get current blendState and blend description of particleEffect (alpha blending off by default)
	ID3D11BlendState *partBS = fireEffect->getBlendState();
	D3D11_BLEND_DESC partBD;
	partBS->GetDesc(&partBD);

	// Modify blend description - alpha blending on
	partBD.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	partBD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	partBD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	partBD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	partBD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	partBD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// Create new blendState
	partBS->Release(); device->CreateBlendState(&partBD, &partBS);
	fireEffect->setBlendState(partBS);

	// get current depthStencil State and depthStencil description of particleEffect (depth read and write by default)
	ID3D11DepthStencilState *partDSS = fireEffect->getDepthStencilState();
	D3D11_DEPTH_STENCIL_DESC partDSD;
	partDSS->GetDesc(&partDSD);

	//Disable Depth Writing for particles
	partDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	// Create custom fire depth-stencil state object
	partDSS->Release(); device->CreateDepthStencilState(&partDSD, &partDSS);
	fireEffect->setDepthStencilState(partDSS);

	// Setup CBuffer
	cBufferExtSrc = (CBufferExt*)_aligned_malloc(sizeof(CBufferExt), 16);
	// Initialise CBuffer
	cBufferExtSrc->worldMatrix = XMMatrixIdentity();
	cBufferExtSrc->worldITMatrix = XMMatrixIdentity();
	cBufferExtSrc->WVPMatrix = mainCamera->getViewMatrix()*mainCamera->getProjMatrix();

	//Used in geometry shader cube map optimisation to set camera view matrices in reflection_map shader cbuffers
	for (int i = 0; i < 6; i++)
		cBufferExtSrc->cameraViewMatrices[i] = renderTargetCameras[i]->getViewMatrix();

	cBufferExtSrc->lightVec = XMFLOAT4(250.0, -130.0, -145.0, 0.0); // Directional light
	cBufferExtSrc->lightAmbient = XMFLOAT4(0.3, 0.3, 0.3, 1.0);
	cBufferExtSrc->lightDiffuse = XMFLOAT4(0.8, 0.8, 0.8, 1.0);
	cBufferExtSrc->lightSpecular = XMFLOAT4(1.0, 1.0, 1.0, 1.0);

	XMStoreFloat4(&cBufferExtSrc->light2Vec, originalLight2Vec); // Positional light
	cBufferExtSrc->light2Ambient = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	cBufferExtSrc->light2Diffuse = XMFLOAT4(0.1, 0.1, 0.4, 1.0);
	cBufferExtSrc->light2Specular = XMFLOAT4(1.0, 1.0, 1.0, 1.0);

	XMStoreFloat4(&cBufferExtSrc->eyePos, mainCamera->getPos());

	D3D11_BUFFER_DESC cbufferDesc;
	D3D11_SUBRESOURCE_DATA cbufferInitData;
	ZeroMemory(&cbufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&cbufferInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	cbufferDesc.ByteWidth = sizeof(CBufferExt);
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferInitData.pSysMem = cBufferExtSrc;

	//Create cBuffers
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferBridge);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferTowerA);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferKnight);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferSkyBox);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferSphere);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferStand);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferWalls);
	hr = device->CreateBuffer(&cbufferDesc, &cbufferInitData, &cBufferFire);

	// Setup example objects

	mattWhite.setSpecular(XMCOLOR(0, 0, 0, 0));
	glossWhite.setSpecular(XMCOLOR(1, 1, 1, 1));

	brickTexture = new Texture(device, L"Resources\\Textures\\brick_DIFFUSE.jpg");
	mossWallTexture = new Texture(device, L"Resources\\Textures\\Moss wall.jpg");
	knightTexture = new Texture(device, L"Resources\\Textures\\knight_orig.jpg");
	envMapTexture = new Texture(device, L"Resources\\Textures\\grassenvmap1024.dds");
	rustDiffTexture = new Texture(device, L"Resources\\Textures\\rustDiff2.jpg");
	rustSpecTexture = new Texture(device, L"Resources\\Textures\\rustSpec2.jpg");
	fireTexture = new Texture(device, L"Resources\\Textures\\Fire.jpg");

	ID3D11ShaderResourceView *sphereTextureArray[] = { rustDiffTexture->SRV, mDynamicCubeMapSRV, rustSpecTexture->SRV };

	//load bridge
	bridge = new Model(device, perPixelLightingEffect, wstring(L"Resources\\Models\\bridge.3ds"), mossWallTexture->SRV, &mattWhite);
	towerA = new Model(device, perPixelLightingEffect, wstring(L"Resources\\Models\\tower.3ds"), mossWallTexture->SRV, &mattWhite);
	knight = new Model(device, perPixelLightingEffect, wstring(L"Resources\\Models\\knight.3ds"), knightTexture->SRV, &mattWhite);
	sphere = new Model(device, refMapEffect, wstring(L"Resources\\Models\\sphere.3ds"), sphereTextureArray, 3, &glossWhite);
	stand = new Model(device, perPixelLightingEffect, wstring(L"Resources\\Models\\stand.3ds"), mossWallTexture->SRV, &mattWhite);
	box = new Box(device, skyBoxEffect, envMapTexture->SRV);
	walls = new Model(device, perPixelLightingEffect, wstring(L"Resources\\Models\\Castle walls.3ds"), mossWallTexture->SRV, &mattWhite);
	fire = new GPUParticles(device, fireEffect, fireTexture->SRV, &mattWhite);

	return S_OK;
}

// Update scene state (perform animations etc)
HRESULT Scene::updateScene(ID3D11DeviceContext *context, FirstPersonCamera* camera) {


	mainClock->tick();
	gu_seconds tDelta = mainClock->gameTimeElapsed();

	cBufferExtSrc->Timer = (FLOAT)tDelta;
	XMStoreFloat4(&cBufferExtSrc->eyePos, camera->getPos());

	// Update bridge cBuffer
	// Scale and translate bridge world matrix
	cBufferExtSrc->worldMatrix = XMMatrixScaling(0.05, 0.05, 0.05)*XMMatrixTranslation(0, -2.7, -15);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferBridge);

	cBufferExtSrc->worldMatrix = XMMatrixScaling(2, 3, 2)*XMMatrixTranslation(0, -5, 15);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferTowerA);

	cBufferExtSrc->worldMatrix = XMMatrixRotationY(1.5) * XMMatrixScaling(0.05, 0.05, 0.05)*XMMatrixTranslation(0, -3, 20) * XMMatrixRotationY(tDelta * 0.1);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferKnight);

	cBufferExtSrc->worldMatrix = XMMatrixScaling(100.0, 100, 100)*XMMatrixTranslation(0, 0, 0);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferSkyBox);

	cBufferExtSrc->worldMatrix = XMMatrixScaling(1.0, 1, 1) * XMMatrixRotationX(tDelta) * sphereTranslationMatrix;
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferSphere);

	cBufferExtSrc->worldMatrix = XMMatrixScaling(0.05, 0.05, 0.05)*XMMatrixTranslation(0, -3, 0);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferStand);

	cBufferExtSrc->worldMatrix = XMMatrixScaling(0.02, 0.02, 0.02)*XMMatrixTranslation(0, -3, -6);
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix*camera->getViewMatrix()*camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferWalls);

	cBufferExtSrc->Timer = cBufferExtSrc->Timer * 3;// speed up particles
	// Scale and translate fire world matrix
	cBufferExtSrc->worldMatrix = XMMatrixScaling(1, 1, 1) * XMMatrixTranslation(-2.5, 1.0, 2.0) * XMMatrixRotationY(tDelta * 0.5) * sphereTranslationMatrix;
	cBufferExtSrc->worldITMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, cBufferExtSrc->worldMatrix));
	cBufferExtSrc->WVPMatrix = cBufferExtSrc->worldMatrix * camera->getViewMatrix() * camera->getProjMatrix();
	mapCbuffer(cBufferExtSrc, cBufferFire);

	XMVECTOR newLight2Vec = XMVector3Transform(originalLight2Vec, XMMatrixRotationY(tDelta * 0.5) * sphereTranslationMatrix);
	XMStoreFloat4(&cBufferExtSrc->light2Vec, newLight2Vec);

	return S_OK;
}

// Helper function to copy cbuffer data from cpu to gpu
HRESULT Scene::mapCbuffer(void *cBufferExtSrcL, ID3D11Buffer *cBufferExtL)
{
	ID3D11DeviceContext *context = dx->getDeviceContext();
	// Map cBuffer
	D3D11_MAPPED_SUBRESOURCE res;
	HRESULT hr = context->Map(cBufferExtL, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	if (SUCCEEDED(hr)) {
		memcpy(res.pData, cBufferExtSrcL, sizeof(CBufferExt));
		context->Unmap(cBufferExtL, 0);
	}
	return hr;
}

// Render scene
HRESULT Scene::renderScene() {

	ID3D11DeviceContext *context = dx->getDeviceContext();

	// Validate window and D3D context
	if (isMinimised() || !context)
		return E_FAIL;

	// Clear the screen
	static const FLOAT clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	// Save current render targets
	ID3D11RenderTargetView* renderTargets[1];
	ID3D11RenderTargetView* defaultRenderTargetView;
	ID3D11DepthStencilView* defaultDepthStencilView;
	context->OMGetRenderTargets(1, &defaultRenderTargetView, &defaultDepthStencilView);

	for (int i = 0; i < 6; i++)
	{
		//rebuild the viewport and update the positions of scene objects from the perspective of the current render target camera
		rebuildRenderTargetViewport(renderTargetCameras[i]);
		updateScene(context, renderTargetCameras[i]);

		// Clear new render target and original depth stencil
		context->ClearRenderTargetView(mDynamicCubeMapRTV[i], clearColor);
		context->ClearDepthStencilView(mDynamicCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Set new render targets - we are using the default depth/stencil as it is the same size
		renderTargets[0] = mDynamicCubeMapRTV[i];
		context->OMSetRenderTargets(1, renderTargets, mDynamicCubeMapDSV);

		renderObjects(context);

		//if (fire) {
		//	fireEffect->bindPipeline(context);

		//	// Apply the particles cBuffer.
		//	context->VSSetConstantBuffers(0, 1, &cBufferFire);
		//	context->GSSetConstantBuffers(0, 1, &cBufferFire);
		//	context->PSSetConstantBuffers(0, 1, &cBufferFire);
		//	// Render
		//	fire->render(context);

		//	context->GSSetShader(NULL, 0, 0);
		//}
	}

	// Restore default render targets
	renderTargets[0] = renderTargetRTV;
	context->OMSetRenderTargets(1, &defaultRenderTargetView, defaultDepthStencilView);

	rebuildViewport(mainCamera);
	updateScene(context, mainCamera);

	// Clear new render target and original depth stencil
	float clearColor2[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(defaultRenderTargetView, clearColor2);
	context->ClearDepthStencilView(defaultDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	renderObjects(context);

	if (sphere) {
		// Apply the sphere cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferSphere);
		context->PSSetConstantBuffers(0, 1, &cBufferSphere);
		// Render
		sphere->render(context);
	}

	//fire must be rendered after sphere, otherwise sphere covers it when fire passes in front of the sphere
	//if (fire) {
	//	fireEffect->bindPipeline(context);

	//	// Apply the particles cBuffer.
	//	context->VSSetConstantBuffers(0, 1, &cBufferFire);
	//	context->GSSetConstantBuffers(0, 1, &cBufferFire);
	//	context->PSSetConstantBuffers(0, 1, &cBufferFire);
	//	// Render
	//	fire->render(context);

	//	context->GSSetShader(NULL, 0, 0);
	//}

	// Present current frame to the screen
	HRESULT hr = dx->presentBackBuffer();

	return S_OK;
}

// Render scene using geometry shader optimisation of the cube map creation (cube map is created in single pass).
// Not working, but does compile and render the sphere.
HRESULT Scene::renderSceneWithCubeMapGS() {

	ID3D11DeviceContext *context = dx->getDeviceContext();

	// Validate window and D3D context
	if (isMinimised() || !context)
		return E_FAIL;

	// Clear the screen
	static const FLOAT clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	// Save current render targets
	ID3D11RenderTargetView* renderTargets[1];
	ID3D11RenderTargetView* defaultRenderTargetView;
	ID3D11DepthStencilView* defaultDepthStencilView;
	context->OMGetRenderTargets(1, &defaultRenderTargetView, &defaultDepthStencilView);

	//
	//Render to cube map render target in single pass
	//
	rebuildViewport(mainCamera);
	updateScene(context, mainCamera);

	// Clear new render target and original depth stencil
	float clearColor2[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(mDynamicCubeMapRTV_SinglePass, clearColor2);
	context->ClearDepthStencilView(mDynamicCubeMapDSV_SinglePass, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* aRTViews[1] = { mDynamicCubeMapRTV_SinglePass };
	context->OMSetRenderTargets(sizeof(aRTViews) / sizeof(aRTViews[0]), aRTViews, mDynamicCubeMapDSV_SinglePass);

	renderObjects(context);

	//fire must be rendered after sphere, otherwise sphere covers it when fire passes in front of the sphere
	if (fire) {
		fireEffect->bindPipeline(context);

		// Apply the particles cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferFire);
		context->GSSetConstantBuffers(0, 1, &cBufferFire);
		context->PSSetConstantBuffers(0, 1, &cBufferFire);
		// Render
		fire->render(context);

		context->GSSetShader(NULL, 0, 0);
	}

	//
	//Render scene to default render target
	//
	renderTargets[0] = renderTargetRTV;
	context->OMSetRenderTargets(1, &defaultRenderTargetView, defaultDepthStencilView);

	rebuildViewport(mainCamera);
	updateScene(context, mainCamera);

	context->ClearRenderTargetView(defaultRenderTargetView, clearColor2);
	context->ClearDepthStencilView(defaultDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	renderObjects(context);

	if (sphere) {
		// Apply the sphere cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferSphere);
		context->PSSetConstantBuffers(0, 1, &cBufferSphere);
		// Render
		sphere->render(context);
	}

	//fire must be rendered after sphere, otherwise sphere covers it when fire passes in front of the sphere
	if (fire) {
		fireEffect->bindPipeline(context);

		// Apply the particles cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferFire);
		context->GSSetConstantBuffers(0, 1, &cBufferFire);
		context->PSSetConstantBuffers(0, 1, &cBufferFire);
		// Render
		fire->render(context);

		context->GSSetShader(NULL, 0, 0);
	}

	// Present current frame to the screen
	HRESULT hr = dx->presentBackBuffer();

	return S_OK;
}

//calls to render objects have been moved from renderScene() to this function, to make the renderScene() code more readable
HRESULT Scene::renderObjects(ID3D11DeviceContext* context)
{
	if (bridge) {

		// Setup pipeline for effect
		// Apply the bridge cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferBridge);
		context->PSSetConstantBuffers(0, 1, &cBufferBridge);
		// Render
		bridge->render(context);
	}

	if (towerA)
	{
		context->VSSetConstantBuffers(0, 1, &cBufferTowerA);
		context->PSSetConstantBuffers(0, 1, &cBufferTowerA);

		towerA->render(context);
	}

	if (knight)
	{
		context->VSSetConstantBuffers(0, 1, &cBufferKnight);
		context->PSSetConstantBuffers(0, 1, &cBufferKnight);

		knight->render(context);
	}

	if (box) {
		// Apply the box cBuffer.
		context->VSSetConstantBuffers(0, 1, &cBufferSkyBox);
		context->PSSetConstantBuffers(0, 1, &cBufferSkyBox);
		// Render
		box->render(context);
	}

	if (walls)
	{
		context->VSSetConstantBuffers(0, 1, &cBufferWalls);
		context->PSSetConstantBuffers(0, 1, &cBufferWalls);

		walls->render(context);
	}

	if (stand)
	{
		context->VSSetConstantBuffers(0, 1, &cBufferStand);
		context->PSSetConstantBuffers(0, 1, &cBufferStand);

		stand->render(context);
	}

	return S_OK;
}