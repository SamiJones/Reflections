
#include <stdafx.h>
#include <Particles.h>
#include <DXVertexExt.h>
#include <iostream>
#include <exception>
#include <Effect.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;





Particles::Particles(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material) {
	
	effect = _effect;
	material = _material;
	inputLayout = effect->getVSInputLayout();
	
	//diffuse = XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// BGRA
	//spec = XMCOLOR(0.0f, 0.0f, 0.0f, 0.0f);// specular power = a * 1000.0
	try
	{

		//INITIALISE Verticies

		for (int i = 0; i<(N_VERT); i += 4)
		{
			vertices[i + 0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 0].posL = XMFLOAT3(-1.0f, -1.0f, 0.0f);
			vertices[i + 0].velocity = XMFLOAT3(((FLOAT)rand() / RAND_MAX) - 0.5, (FLOAT)rand() / RAND_MAX, ((FLOAT)rand() / RAND_MAX) - 0.5);
			vertices[i + 0].data = XMFLOAT3((FLOAT)rand() / RAND_MAX, 0.0f, 0.0f);

			vertices[i + 1].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 1].posL = XMFLOAT3(-1.0f, 1.0f, 0.0f);
			vertices[i + 1].velocity = vertices[i + 0].velocity;
			vertices[i + 1].data = vertices[i + 0].data;

			vertices[i + 2].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 2].posL = XMFLOAT3(1.0f, 1.0f, 0.0f);
			vertices[i + 2].velocity = vertices[i + 0].velocity;
			vertices[i + 2].data = vertices[i + 0].data;

			vertices[i + 3].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 3].posL = XMFLOAT3(1.0f, -1.0f, 0.0f);
			vertices[i + 3].velocity = vertices[i + 0].velocity;
			vertices[i + 3].data = vertices[i + 0].data;

		}

	
		// Setup particles vertex buffer


		if (!device || !inputLayout)
			throw exception("Invalid parameters for particles instantiation");

		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexdata;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexdata, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexParticle) * N_VERT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexdata.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexdata, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

		// Create the index buffer

		UINT indices[N_P_IND];

		//INITIALISE Indicies

		for (int i = 0; i<N_PART; i++)
		{

			indices[(i * 6) + 0] = (i * 4) + 2;
			indices[(i * 6) + 1] = (i * 4) + 1;
			indices[(i * 6) + 2] = (i * 4) + 0;

			indices[(i * 6) + 3] = (i * 4) + 0;
			indices[(i * 6) + 4] = (i * 4) + 3;
			indices[(i * 6) + 5] = (i * 4) + 2;
		}


		D3D11_BUFFER_DESC indexDesc;
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.ByteWidth = sizeof(UINT) * N_P_IND;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA indexdata;
		indexdata.pSysMem = indices;
		
		hr = device->CreateBuffer(&indexDesc, &indexdata, &indexBuffer);
		
		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");


		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexParticle::createInputLayout(device, vsBytecode, &inputLayout);
		//if (!SUCCEEDED(hr))
		//	throw exception("Cannot create input layout interface");


		textureResourceView = tex_view;

		if (textureResourceView)
			textureResourceView->AddRef(); // We didnt create it here but dont want it deleted by the creator untill we have deconstructed

		D3D11_SAMPLER_DESC samplerDesc;

		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy=16;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = 0.0f;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateSamplerState(&samplerDesc, &linearSampler);

	


	}
	catch (exception& e)
	{
		cout << "Particles object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		inputLayout = nullptr;
		indexBuffer = nullptr;
	}
}

void Particles::setTexture(ID3D11ShaderResourceView *tex_view)
{
	if (textureResourceView)
		textureResourceView->Release();
	textureResourceView = tex_view;

	if (textureResourceView)
		textureResourceView->AddRef(); // We didnt create it here but dont want it deleted by the creator untill we have deconstructed

}
Particles::~Particles() {

	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();
	if (inputLayout)
		inputLayout->Release();

	if (textureResourceView)
		textureResourceView->Release();

	if (linearSampler)
		linearSampler->Release();
}


void Particles::render(ID3D11DeviceContext *context) {

	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !effect)
		return;

	effect->bindPipeline(context);

	// set shaders for effect
	context->VSSetShader(effect->getVertexShader(), 0, 0);
	context->PSSetShader(effect->getPixelShader(), 0, 0);

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexParticle) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (textureResourceView && linearSampler) {

		context->PSSetShaderResources(0, 1, &textureResourceView);
		context->PSSetSamplers(0, 1, &linearSampler);
	}

	// Draw particles object using index buffer
	// indices for the particles.
	context->DrawIndexed(N_P_IND, 0, 0);
}

