
#include <stdafx.h>
#include <Box.h>
#include <DXVertexExt.h>
#include <iostream>
#include <exception>
#include <Effect.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


#pragma region Box model data

XMCOLOR		diffuse =		XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// BGRA
XMCOLOR		spec =			XMCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// specular power = a * 1000.0


static const DXVertexExt vertices[] = {
	  //|pos,						  | normal				|matDiffuse|matSpecular|texCoord
		//Front face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) }, //0
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) }, //1
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) }, //2
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) }, //3
		
		//Back face
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(0.0f, 0.0f, +1.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) }, //4
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(0.0f, 0.0f, +1.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) }, //5
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(0.0f, 0.0f, +1.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) }, //6
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(0.0f, 0.0f, +1.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) }, //7

		//Left Face duplicate 4, 5, 1, 4, 1, 0 -> 8,9,10,8,10,11
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) }, //4->8
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) }, //5->9
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) }, //1->10
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) }, //0->11

		//Right Face duplicate 3, 2, 6, 3, 6, 7, ->12,13,14,12,14,15,
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(+1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) },//3->12
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(+1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) },//2->13
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(+1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) },//6->14
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(+1.0f, 0.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) }, //7->15
		
		//Top Face duplicate1, 5, 6, 1, 6, 2 ->16,17,18,16,18,19,
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) }, //1->16
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) }, //5->17
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) }, //6->18
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) }, //2->19

		//Bottom Face duplicate4, 0, 3, 4, 3, 7 -> 20,21,22,20,22,23,
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 1.0f) }, //4->20
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), diffuse, spec, XMFLOAT2(0.0f, 0.0f) }, //0->21
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 0.0f) }, //3->22
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), diffuse, spec, XMFLOAT2(1.0f, 1.0f) } //7->23
};

// Create the indices
UINT indices[] = {

		// front face OK
		0, 1, 2,
		0, 2, 3,

		// back face OK
		4, 6, 5,
		4, 7, 6,

		// left face
		//4, 5, 1,4, 1, 0,
		8, 9, 10,
		8, 10, 11,

		// right face
		//3, 2, 6,3, 6, 7,
		12, 13, 14,
		12, 14, 15,

		// top face
		//1, 5, 6,1, 6, 2,
		16, 17, 18,
		16, 18, 19,

		// bottom face
		//4, 0, 3, 4, 3, 7
		20, 21, 22,
		20, 22, 23,
};


#pragma endregion
void Box::setTexture(ID3D11ShaderResourceView *tex_view)
{
	textureResourceView = tex_view;
}

Box::Box(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view) {
	effect = _effect;
	try
	{

		// Setup triangle vertex buffer

		if (!device || !effect)
			throw exception("Invalid parameters for triangle model instantiation");

		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexExt) * 24;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

		D3D11_BUFFER_DESC indexDesc;
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.ByteWidth = sizeof(UINT) * 36;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices;
		
		hr = device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);
		
		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");


		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexExt::createInputLayout(device, *vsBytecode, &inputLayout);
		//if (!SUCCEEDED(hr))
		//	throw exception("Cannot create input layout interface");


		textureResourceView = tex_view;

		if (textureResourceView)
			textureResourceView->AddRef(); // We didnt create it here but dont want it deleted by the creator untill we have deconstructed

		D3D11_SAMPLER_DESC linearDesc;

		ZeroMemory(&linearDesc, sizeof(D3D11_SAMPLER_DESC));

		linearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		linearDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		linearDesc.MinLOD = 0.0f;
		linearDesc.MaxLOD = 0.0f;
		linearDesc.MipLODBias = 0.0f;
		//linearDesc.MaxAnisotropy = 0; // Unused for isotropic filtering
		linearDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&linearDesc, &sampler);

	}
	catch (exception& e)
	{
		cout << "Box object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
	}
}


Box::~Box() {

	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();
	//if (effect)
	//	effect->Release();

	if (textureResourceView)
		textureResourceView->Release();

	if (sampler)
		sampler->Release();

}


void Box::render(ID3D11DeviceContext *context) {

	// set shader reflection map shaders for shark
	context->VSSetShader(effect->getVertexShader(), 0, 0);
	context->PSSetShader(effect->getPixelShader(), 0, 0);
	
	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexExt) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (textureResourceView && sampler) {

		context->PSSetShaderResources(0, 1, &textureResourceView);
		context->PSSetSamplers(0, 1, &sampler);
	}
	effect->bindPipeline(context);
	// Draw box object using index buffer
	// 36 indices for the box.
	context->DrawIndexed(36, 0, 0);
}
