
//
// DXSystem.h
//

// Model a singleton class that encapsulates D3D device independent, device dependent and window size dependent resources.  For now this supports a single swap chain so there exists a 1:1 correspondance between the DXSystem instance and the associated window.

#pragma once

#include <d3d11_2.h>
#include <windows.h>
#include <GUObject.h>



// Note: By default we use DXGI 1.3.  Comment out this line to use DXGI 1.1 on Windows 7
// #define __USE_DXGI_1_3__			1


class DXSystem : public GUObject {

#ifdef __USE_DXGI_1_3__

	// DXGI 1.3
	IDXGIFactory2							*dxgiFactory = nullptr;
	IDXGISwapChain1							*swapChain = nullptr;
	DXGI_PRESENT_PARAMETERS					dxgiPresentParams;

#else

	// Use DXGI 1.1
	IDXGIFactory1							*dxgiFactory = nullptr;
	IDXGISwapChain							*swapChain = nullptr;

#endif

	IDXGIAdapter							*defaultAdapter = nullptr;
	ID3D11Device							*device = nullptr;
	ID3D11DeviceContext						*context = nullptr;
	D3D_FEATURE_LEVEL						supportedFeatureLevel;
	
	ID3D11RenderTargetView *renderTargetView = nullptr;
	ID3D11DepthStencilView		*depthStencilView = nullptr;

	ID3D11Texture2D				*depthStencilBuffer = nullptr;

	//
	// Private interface
	//

	// Private constructor
	DXSystem(HWND hwnd);


public:

	//
	// Public interface
	//

	// DXSystem factory method
	static DXSystem* CreateDirectXSystem(HWND hwnd);

	// Destructor
	~DXSystem();


	// Setup DirectX interfaces that will be constant - they do not depend on any device
	HRESULT setupDeviceIndependentResources();

	// Setup DirectX interfaces that are dependent upon a specific device
	HRESULT setupDeviceDependentResources(HWND hwnd);

	// Setup window-specific resources including swap chain buffers, texture buffers and resource views that are dependant upon the host window size
	HRESULT setupWindowDependentResources(HWND hwnd);


	// Update methods

	// Resize swap chain buffers according to the given window client area
	HRESULT resizeSwapChainBuffers(HWND hwnd);

	// Present back buffer to the screen
	HRESULT presentBackBuffer();


	// Accessor methods
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11RenderTargetView* getBackBufferRTV();
	ID3D11DepthStencilView* getDepthStencil();
	ID3D11Texture2D* getDepthStencilBuffer();
};
