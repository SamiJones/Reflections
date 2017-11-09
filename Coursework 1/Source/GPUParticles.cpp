
#include <stdafx.h>
#include <GPUParticles.h>
#include <DXVertexExt.h>
#include <iostream>
#include <exception>
#include <Effect.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;





GPUParticles::GPUParticles(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material) {

	effect = _effect;
	material = _material;
	inputLayout = effect->getVSInputLayout();


	try
	{

		//INITIALISE Verticies

		for (int i = 0; i<(N_PART); i++)
		{
			vertices[i + 0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vertices[i + 0].posL = XMFLOAT3(-1.0f, -1.0f, 0.0f);//notused
			vertices[i + 0].velocity = XMFLOAT3(((FLOAT)rand() / RAND_MAX) - 0.5, (FLOAT)rand() / RAND_MAX, ((FLOAT)rand() / RAND_MAX) - 0.5);
			vertices[i + 0].data = XMFLOAT3((FLOAT)rand() / RAND_MAX, 0.0f, 0.0f);
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
		vertexDesc.ByteWidth = sizeof(DXVertexParticle) * N_PART;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexdata.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexdata, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");




		textureResourceView = tex_view;

		if (textureResourceView)
			textureResourceView->AddRef(); // We didnt create it here but dont want it deleted by the creator untill we have deconstructed

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


void GPUParticles::render(ID3D11DeviceContext *context) {

	// Validate object before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !effect)
		return;

	effect->bindPipeline(context);

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexParticle) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);


	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (textureResourceView && linearSampler) {

		context->PSSetShaderResources(0, 1, &textureResourceView);
		context->PSSetSamplers(0, 1, &linearSampler);
	}

	// Draw particles object using Geometry Shader

	context->Draw(N_PART, 0);
}

