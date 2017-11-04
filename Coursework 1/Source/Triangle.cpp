
#include <stdafx.h>
#include <Triangle.h>
#include <DXVertexBasic.h>
#include <iostream>
#include <exception>


using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


#pragma region Triangle model data

static const DXVertexBasic vertices[] = {

		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }
};

#pragma endregion


Triangle::Triangle(ID3D11Device *device, ID3D11InputLayout *_inputLayout) {


	inputLayout = _inputLayout;
	try
	{
		// Setup triangle vertex buffer

		if (!device || !inputLayout)
			throw exception("Invalid parameters for triangle model instantiation");

		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexBasic) * 3;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexBasic::createInputLayout(device, vsBytecode, &inputLayout);
		//if (!SUCCEEDED(hr))
		//	throw exception("Cannot create input layout interface");
	}
	catch (exception& e)
	{
		cout << "Triangle object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		inputLayout = nullptr;
	}
}


Triangle::~Triangle() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (inputLayout)
		inputLayout->Release();
}


void Triangle::render(ID3D11DeviceContext *context) {

	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !inputLayout)
		return;

	// Set vertex layout
	context->IASetInputLayout(inputLayout);

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexBasic) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw triangle object
	// Note: Draw vertices in the buffer one after the other.  Not the most efficient approach (see duplication in the above vertex data)
	// This is shown here for demonstration purposes
	context->Draw(3, 0);
}
