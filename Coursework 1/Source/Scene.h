
//
// Scene.h

#pragma once

#include <GUObject.h>
#include <Windows.h>
#include <Box.h>
#include <Triangle.h>
#include <Particles.h>
#include <Quad.h>

#include <CBufferStructures.h>
#include <Material.h>

class DXSystem;
class CGDClock;
class Model;
class Camera;
class LookAtCamera;
class FirstPersonCamera;
class Texture;
class Effect;





class Scene : public GUObject {

	HINSTANCE								hInst = NULL;
	HWND									wndHandle = NULL;

	// Strong reference to associated Direct3D device and rendering context.
	DXSystem								*dx = nullptr;

	D3D11_VIEWPORT							viewport;
	D3D11_VIEWPORT							renderTargetViewport;

	Material								mattWhite;
	Material								glossWhite;



	// Direct3D scene textures and resource views

	//Effects
	Effect									*defaultEffect;
	Effect									*perPixelLightingEffect;
	Effect									*skyBoxEffect;
	Effect									*basicEffect;
	Effect									*refMapEffect;
	Effect									*fireEffect;
	
	ID3D11Buffer							*cBufferSkyBox = nullptr;
	ID3D11Buffer							*cBufferBridge = nullptr;
	ID3D11Buffer							*cBufferTowerA = nullptr;
	ID3D11Buffer							*cBufferKnight = nullptr;
	ID3D11Buffer							*cBufferTree = nullptr;
	ID3D11Buffer							*cBufferSphere = nullptr;
	ID3D11Buffer							*cBufferStand = nullptr;
	ID3D11Buffer							*cBufferWalls = nullptr;
	ID3D11Buffer							*cBufferFire = nullptr;

	CBufferExt								*cBufferExtSrc = nullptr;

	Texture									*brickTexture = nullptr;
	Texture									*mossWallTexture = nullptr;
	Texture									*rustDiffTexture = nullptr;
	Texture									*rustSpecTexture = nullptr;
	Texture									*envMapTexture = nullptr;
	Texture									*knightTexture = nullptr;
	Texture									*fireTexture = nullptr;

	// Tutorial 04
	ID3D11ShaderResourceView*				mDynamicCubeMapSRV;
	ID3D11RenderTargetView*					renderTargetRTV;
	ID3D11RenderTargetView*					mDynamicCubeMapRTV[6];
	ID3D11DepthStencilView*					mDynamicCubeMapDSV;

	//used for the (attempted) efficient creation of cube map using the geometry shader in a single pass
	ID3D11RenderTargetView*					mDynamicCubeMapRTV_SinglePass;
	ID3D11DepthStencilView*					mDynamicCubeMapDSV_SinglePass;
	
	//Models
	Model									*bridge = nullptr;
	Model									*towerA = nullptr;
	Model									*knight = nullptr;
	Box										*box = nullptr;
	Model									*sphere = nullptr;
	Model									*stand = nullptr;
	Quad									*triangle = nullptr;
	Model									*walls = nullptr;
	Particles								*fire = nullptr;
	// Main FPS clock
	CGDClock								*mainClock = nullptr;

	//Main scene camera
	FirstPersonCamera						*mainCamera = nullptr;

	//Cameras used for rendering to 6 render targets for the dynamic reflection
	FirstPersonCamera						*renderTargetCameras[6]; 

	//the size of each face of the cube map texture - a low resolution such as 256 x 256 saves processing
	const int								CUBEMAP_SIZE = 256;

	//used for applying a user-defined translation to the reflective sphere in updateScene()
	DirectX::XMMATRIX						sphereTranslationMatrix = XMMatrixIdentity();
	
	//initial position of the second light in the scene (coming from the fire)
	DirectX::XMVECTOR						originalLight2Vec = DirectX::XMVectorSet(-2.5, 0.0, 2.0, 1.0);

	//
	// Private interface
	//

	// Private constructor
	Scene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc);

	// Return TRUE if the window is in a minimised state, FALSE otherwise
	BOOL isMinimised();


public:

	//
	// Public interface
	//
	// Factory method to create the main Scene instance (singleton)
	static Scene* CreateScene(const LONG _width, const LONG _height, const wchar_t* wndClassName, const wchar_t* wndTitle, int nCmdShow, HINSTANCE hInstance, WNDPROC WndProc);

	// Destructor
	~Scene();

	// Decouple the encapsulated HWND and call DestoryWindow on the HWND
	void destoryWindow();

	// Resize swap chain buffers and update pipeline viewport configurations in response to a window resize event
	HRESULT resizeResources();

	// Helper function to call updateScene followed by renderScene
	HRESULT updateAndRenderScene();
	HRESULT mapCbuffer(void *cBufferExtSrcL, ID3D11Buffer *cBufferExtL);
	// Clock handling methods
	void startClock();
	void stopClock();
	void reportTimingData();


	//
	// Event handling methods
	//

	// Process mouse move with the left button held down
	void handleMouseLDrag(const POINT &disp);

	// Process mouse wheel movement
	void handleMouseWheel(const short zDelta);

	// Process key down event.  keyCode indicates the key pressed while extKeyFlags indicates the extended key status at the time of the key down event (see http://msdn.microsoft.com/en-gb/library/windows/desktop/ms646280%28v=vs.85%29.aspx).
	void handleKeyDown(const WPARAM keyCode, const LPARAM extKeyFlags);
	
	// Process key up event.  keyCode indicates the key released while extKeyFlags indicates the extended key status at the time of the key up event (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms646281%28v=vs.85%29.aspx).
	void handleKeyUp(const WPARAM keyCode, const LPARAM extKeyFlags);

	

	//
	// Methods to handle initialisation, update and rendering of the scene
	//
	
	//Additional rebuildRenderTargetViewport() function added because the render targets for the cube map faces require a different viewport size.
	//They also require a different projection matrix to widen the camera view - this removes the "blind spots" from the final cube map used for the reflection
	HRESULT rebuildViewport(Camera *camera);
	HRESULT rebuildRenderTargetViewport(Camera* camera);

	HRESULT initDefaultPipeline();
	HRESULT bindDefaultPipeline();
	HRESULT LoadShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader);
	uint32_t LoadShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader);
	HRESULT initialiseSceneResources();
	HRESULT updateScene(ID3D11DeviceContext *context, FirstPersonCamera* camera); //updates cbuffers using the view and projection matrices of the specified camera
	HRESULT renderScene();
	HRESULT renderSceneWithCubeMapGS();
	HRESULT renderObjects(ID3D11DeviceContext* context);

	void DrawScene(ID3D11DeviceContext *context);



};
