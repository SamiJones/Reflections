
#include <stdafx.h>
#include <Quad.h>
#include <DXVertexBasic.h>
#include <iostream>
#include <exception>


using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


#pragma region Quad model data

static const DXVertexBasic vertices[] = {

		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }
};

#pragma endregion


Quad::Quad(ID3D11Device *device, ID3D11InputLayout	*_inputLayout) {

	try
	{
		// Setup triangle vertex buffer
		inputLayout = _inputLayout;
		if (!device || !inputLayout)
			throw exception("Invalid parameters for triangle model instantiation");

		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexBasic) * 6;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexBasic::createInputLayout(device, vsBytecode, &inputLayout);
		//if (!SUCCEEDED(hr))
		//	throw exception("Cannot create input layout interface");


		D3D11_SAMPLER_DESC samplerDesc;

		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = 0.0f;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&samplerDesc, &linearSampler);



	}
	catch (exception& e)
	{
		cout << "Quad object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		inputLayout = nullptr;
	}
}


Quad::~Quad() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (inputLayout)
		inputLayout->Release();

	if (linearSampler)
		linearSampler->Release();
}


void Quad::render(ID3D11DeviceContext *context) {

	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !inputLayout)
		return;

	// Set vertex layout
	context->IASetInputLayout(inputLayout);

	//// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexBasic) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (linearSampler) {

		context->PSSetSamplers(0, 1, &linearSampler);

	}

	// Draw triangle object
	// Note: Draw vertices in the buffer one after the other.  Not the most efficient approach (see duplication in the above vertex data)
	// This is shown here for demonstration purposes
	context->Draw(6, 0);
}
