#include "stdafx.h"
#include "Effect.h"
//#include <string.h>
//
//#include <Windows.h>

using namespace std;

void Effect::bindPipeline(ID3D11DeviceContext *context){
	context->RSSetState(RasterizerState);
	// Apply dsState
	context->OMSetDepthStencilState(DepthStencilState, 0);
	context->OMSetBlendState(BlendState, blendFactor, sampleMask);
	context->VSSetShader(VertexShader, 0, 0);
	context->PSSetShader(PixelShader, 0, 0);
	context->GSSetShader(GeometryShader, 0, 0);
}

void Effect::initDefaultStates(ID3D11Device *device ){
	
	D3D11_RASTERIZER_DESC			RSdesc;
	ZeroMemory(&RSdesc, sizeof(D3D11_RASTERIZER_DESC));
	// Setup default rasteriser state 
	RSdesc.FillMode = D3D11_FILL_SOLID;
	RSdesc.CullMode = D3D11_CULL_NONE;
	RSdesc.FrontCounterClockwise = TRUE;
	RSdesc.DepthBias = 0;
	RSdesc.SlopeScaledDepthBias = 0.0f;
	RSdesc.DepthBiasClamp = 0.0f;
	RSdesc.DepthClipEnable = TRUE;
	RSdesc.ScissorEnable = FALSE;
	RSdesc.MultisampleEnable = TRUE;
	RSdesc.AntialiasedLineEnable = FALSE;
	HRESULT hr = device->CreateRasterizerState(&RSdesc, &RasterizerState);

	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create Rasterise state interface");

	// Output - Merger Stage

	// Initialise default depth-stencil state object
	D3D11_DEPTH_STENCIL_DESC	dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	// Setup default depth-stencil descriptor
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	// Initialise depth-stencil state object based on the given descriptor
	hr = device->CreateDepthStencilState(&dsDesc, &DepthStencilState);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create DepthStencil state interface");

	// Initialise default blend state object (Alpha Blending On)
	D3D11_BLEND_DESC	blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	// Setup default blend state descriptor 
	// Add Code Here (Set Alpha Blending On)
	//blendDesc.AlphaToCoverageEnable = FALSE; // Use pixel coverage info from rasteriser (default)
	//blendDesc.IndependentBlendEnable = FALSE; // The following array of render target blend properties uses the blend properties from RenderTarget[0] for ALL render targets
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	blendDesc.AlphaToCoverageEnable = FALSE; // Use pixel coverage info from rasteriser (default FALSE)
	blendDesc.IndependentBlendEnable = FALSE; // The following array of render target blend properties uses the blend properties from RenderTarget[0] for ALL render targets
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//blendDesc.AlphaToCoverageEnable = FALSE; // Use pixel coverage info from rasteriser (default FALSE)
	//blendDesc.IndependentBlendEnable = FALSE; // The following array of render target blend properties uses the blend properties from RenderTarget[0] for ALL render targets
	//blendDesc.RenderTarget[0].BlendEnable = FALSE;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;



	// Create blendState
	hr = device->CreateBlendState(&blendDesc, &BlendState);
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create Blend state interface");

	blendFactor[0] = blendFactor[1] = blendFactor[2] = blendFactor[3] = 1.0f;
	sampleMask = 0xFFFFFFFF; // Bitwise flags to determine which samples to process in an MSAA context
}
Effect::Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11InputLayout *_VSInputLayout)
{
	VertexShader = _VertexShader;
	PixelShader = _PixelShader;
	GeometryShader = NULL;
	VSInputLayout = _VSInputLayout;
	initDefaultStates(device);
	VertexShader->AddRef();
	PixelShader->AddRef();
	VSInputLayout->AddRef();

}

Effect::Effect(ID3D11Device *device, ID3D11VertexShader	*_VertexShader, ID3D11PixelShader *_PixelShader, ID3D11GeometryShader *_GeometryShader, ID3D11InputLayout *_VSInputLayout)
{
	VertexShader = _VertexShader;
	PixelShader = _PixelShader;
	GeometryShader = _GeometryShader;
	VSInputLayout = _VSInputLayout;
	initDefaultStates(device);
	VertexShader->AddRef();
	PixelShader->AddRef();
	VSInputLayout->AddRef();

}
Effect::Effect(ID3D11Device *device, const char *vertexShaderPath, const char * pixelShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements)
{
	char *tmpShaderBytecode = nullptr;
	uint32_t tmpVSSizeBytes = CreateVertexShader(device, vertexShaderPath, &tmpShaderBytecode, &VertexShader);
	device->CreateInputLayout(vertexDesc, numVertexElements, tmpShaderBytecode, tmpVSSizeBytes, &VSInputLayout);
	CreatePixelShader(device, pixelShaderPath, &tmpShaderBytecode, &PixelShader);
	initDefaultStates(device);
}

Effect::Effect(ID3D11Device *device, const char *vertexShaderPath, const char * pixelShaderPath, const char * geometryShaderPath, const D3D11_INPUT_ELEMENT_DESC vertexDesc[], UINT numVertexElements)
{
	char *tmpShaderBytecode = nullptr;
	uint32_t tmpVSSizeBytes = CreateVertexShader(device, vertexShaderPath, &tmpShaderBytecode, &VertexShader);
	device->CreateInputLayout(vertexDesc, numVertexElements, tmpShaderBytecode, tmpVSSizeBytes, &VSInputLayout);
	CreatePixelShader(device, pixelShaderPath, &tmpShaderBytecode, &PixelShader);
	CreateGeometryShader(device, geometryShaderPath, &tmpShaderBytecode, &GeometryShader);
	initDefaultStates(device);
}

Effect::~Effect()
{
	if (RasterizerState)
		RasterizerState->Release();
	if (DepthStencilState)
		DepthStencilState->Release();
	if (BlendState)
		BlendState->Release();
	if (VertexShader)
		VertexShader->Release();
	if (PixelShader)
		PixelShader->Release();
	if (VSInputLayout)
		VSInputLayout->Release();



}
HRESULT Effect::CreateGeometryShader(ID3D11Device *device, const char *filename, char **GSBytecode, ID3D11GeometryShader **geometryShader){

	char *GSBytecodeLocal;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, &GSBytecodeLocal);

	GSBytecode = &GSBytecodeLocal;
	cout << "Done: GShader memory bytes = " << shaderBytes << endl;
	// Create shader objects
	HRESULT hr = device->CreateGeometryShader(GSBytecodeLocal, shaderBytes, NULL, geometryShader);

	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create GeometryShader interface");
	return hr;
}
HRESULT Effect::CreatePixelShader(ID3D11Device *device, const char *filename, char **PSBytecode, ID3D11PixelShader **pixelShader){

	char *PSBytecodeLocal;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, &PSBytecodeLocal);

	PSBytecode = &PSBytecodeLocal;
	cout << "Done: PShader memory bytes = " << shaderBytes << endl;
	// Create shader objects
	HRESULT hr = device->CreatePixelShader(PSBytecodeLocal, shaderBytes, NULL, pixelShader);

	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create PixelShader interface");
	return hr;
}
uint32_t Effect::CreateVertexShader(ID3D11Device *device, const char *filename, char **VSBytecode, ID3D11VertexShader **vertexShader){

	char *VSBytecodeLocal;

	//Load the compiled shader byte code.
	uint32_t shaderBytes = LoadShader(filename, &VSBytecodeLocal);
	//VSBytecode =(char*) malloc(shaderBytes);
	cout << "Done: VShader memory bytes = " << shaderBytes << endl;
	//for (int i = 0; i < shaderBytes; i++)
	//	*VSBytecode[i] = 'a';
	*VSBytecode = VSBytecodeLocal;
	cout << "Done: VShader writting = " << shaderBytes << endl;
	HRESULT hr = device->CreateVertexShader(VSBytecodeLocal, shaderBytes, NULL, vertexShader);
	cout << "Done: VShader return = " << hr << endl;
	if (!SUCCEEDED(hr))
		throw std::exception("Cannot create VertexShader interface");
	return shaderBytes;
}
uint32_t Effect::LoadShader(const char *filename, char **bytecode)
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