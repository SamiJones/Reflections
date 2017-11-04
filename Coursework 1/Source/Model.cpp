
#include "stdafx.h"
#include <Model.h>
#include <Material.h>
#include <Effect.h>
#include <iostream>
#include <exception>
#include <CoreStructures\CoreStructures.h>
#include <CGImport3\CGModel\CGModel.h>
#include <CGImport3\Importers\CGImporters.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace CoreStructures;

Model::Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *tex_view, Material *_material) {
	
	Num_Textures = 1;
	load(device, _effect, filename, tex_view, _material);
}

Model::Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *_tex_view_array[],int _num_textures, Material *_material) {
	
	load(device, _effect, filename, _tex_view_array[0], _material);
	Num_Textures = min(8,_num_textures);
	for (int i=1; i < Num_Textures; i++)
	{
		textureResourceViewArray[i] = _tex_view_array[i];
	}


}	

void Model::load(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *tex_view, Material *_material) {
	printf("entering model init");
	
	effect = _effect;
	inputLayout = effect->getVSInputLayout();
	inputLayout->AddRef();
	material = _material;
	worldMatrix = XMMatrixIdentity();
	
	CGModel *actualModel = nullptr;
	DXVertexExt *_vertexBuffer = nullptr;
	uint32_t *_indexBuffer = nullptr;

	try
	{
		if (!device || !inputLayout)
			throw exception("Invalid parameters for Model instantiation");

		actualModel = new CGModel();

		if (!actualModel)
			throw exception("Cannot create model to import");

		// Get filename extension
		wstring ext = filename.substr(filename.length() - 4);

		CG_IMPORT_RESULT cg_err;

		if (0 == ext.compare(L".gsf"))
			cg_err = importGSF(filename.c_str(), actualModel);
		else if (0 == ext.compare(L".3ds"))
			cg_err = import3DS(filename.c_str(), actualModel);
		else if (0 == ext.compare(L".obj"))
			cg_err = importOBJ(filename.c_str(), actualModel);
		else
			throw exception("Object file format not supported");

		if (cg_err != CG_IMPORT_OK)
			throw exception("Could not load model");


		// Build a buffer for each mesh
		
		// Generate a single buffer object that stores a CGVertexExt struct per vertex
		// Each CGPolyMesh in actualModel is stored contiguously in the buffer.
		// The indices are also stored in the same way but no offset to each vertex sub-buffer is added.
		// Model stores a vector of base vertex offsets to point to the start of each sub-buffer and start index offsets for each sub-mesh

		numMeshes = actualModel->getMeshCount();

		if (numMeshes == 0)
			throw exception("Empty model loaded");

		uint32_t numVertices = 0;
		uint32_t numIndices = 0;

		for (uint32_t i = 0; i < numMeshes; ++i) {

			// Store base vertex index;
			baseVertexOffset.push_back(numVertices);
			
			CGPolyMesh *M = actualModel->getMeshAtIndex(i);

			if (M) {

				// Increment vertex count
				numVertices += M->vertexCount();

				// Store num indices for current mesh
				indexCount.push_back(M->faceCount() * 3);
				numIndices += M->faceCount() * 3;
			}
		}
		

		// Create vertex buffer
		_vertexBuffer = (DXVertexExt*)malloc(numVertices * sizeof(DXVertexExt));

		if (!_vertexBuffer)
			throw exception("Cannot create vertex buffer");


		// Create index buffer
		_indexBuffer = (uint32_t*)malloc(numIndices * sizeof(uint32_t));

		if (!_indexBuffer)
			throw exception("Cannot create index buffer");


		// Copy vertex data into single buffer
		DXVertexExt *vptr = _vertexBuffer;
		uint32_t *indexPtr = _indexBuffer;

		for (uint32_t i = 0; i < numMeshes; ++i) {

			// Get mesh data (assumes 1:1 correspondance between vertex position, normal and texture coordinate data)
			CGPolyMesh *M = actualModel->getMeshAtIndex(i);

			if (M) {

				CGBaseMeshDefStruct R;
				M->createMeshDef(&R);

				for (uint32_t k = 0; k < uint32_t(R.N); ++k, vptr++) {

					vptr->pos = XMFLOAT3(-R.V[k].x, R.V[k].y, R.V[k].z);
					vptr->normal = XMFLOAT3(R.Vn[k].x, R.Vn[k].y, R.Vn[k].z);
					//vptr->normal.x = -vptr->normal.x;
					//Flip normal.x for OBJ & GSF (might be required for other files too?)
					if (0 == ext.compare(L".obj") || 0 == ext.compare(L".gsf") || 0 == ext.compare(L".3ds"))
						vptr->normal.x = -vptr->normal.x;

					if (R.Vt && R.VtSize > 0)
						vptr->texCoord = XMFLOAT2(R.Vt[k].s, 1.0f - R.Vt[k].t);
					else
						vptr->texCoord = XMFLOAT2(0.0f, 0.0f);

					vptr->matDiffuse = material->getColour()->diffuse;//XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					vptr->matSpecular = material->getColour()->specular;// XMCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// Copy mesh indices from CGPolyMesh into buffer
				memcpy(indexPtr, R.Fv, R.n * sizeof(CGFaceVertex));

				// Re-order indices to account for DirectX using the left-handed coordinate system
				for (int k = 0; k < R.n; ++k, indexPtr += 3)
					swap(indexPtr[0], indexPtr[2]);
			}
		}

		
		//
		// Setup DX vertex buffer interfaces
		//

		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = numVertices * sizeof(DXVertexExt);
		vertexData.pSysMem = _vertexBuffer;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");


		// Setup index buffer
		D3D11_BUFFER_DESC indexDesc;
		D3D11_SUBRESOURCE_DATA indexData;

		ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));

		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = numIndices * sizeof(uint32_t);
		indexData.pSysMem = _indexBuffer;

		hr = device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Index buffer cannot be created");

		// Build the vertex input layout - this is done here since each object may load it's data into the IA differently.  This requires the compiled vertex shader bytecode.
		//hr = DXVertexExt::createInputLayout(device, vsBytecode, &inputLayout);

		if (!SUCCEEDED(hr))
			throw exception("Cannot create input layout interface");
		


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

		// Setup texture interfaces
		textureResourceViewArray[0] = tex_view;
		if (textureResourceViewArray[0])
			textureResourceViewArray[0]->AddRef();




		// Dispose of local resources
		free(_vertexBuffer);
		free(_indexBuffer);
		actualModel->release();
	}
	catch (exception& e)
	{
		cout << "Model could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (_vertexBuffer)
			free(_vertexBuffer);

		if (_indexBuffer)
			free(_indexBuffer);

		if (actualModel)
			actualModel->release();


		if (vertexBuffer)
			vertexBuffer->Release();

		if (indexBuffer)
			indexBuffer->Release();

		if (inputLayout)
			inputLayout->Release();

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		inputLayout = nullptr;

		numMeshes = 0;
	}
}



Model::~Model() {

}

//void Model::update(ID3D11DeviceContext *context) {

void Model::render(ID3D11DeviceContext *context) {

	effect->bindPipeline(context);

	//// set  shaders for effect
	context->VSSetShader(effect->getVertexShader(), 0, 0);
	context->PSSetShader(effect->getPixelShader(), 0, 0);

	// Validate Model before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !indexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set Model vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexExt) };
	UINT vertexOffsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology for IA
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Bind texture resource views and texture sampler objects to the PS stage of the pipeline
	if (textureResourceViewArray[0] && sampler) {

		context->PSSetShaderResources(0, Num_Textures, textureResourceViewArray);
		context->PSSetSamplers(0, 1, &sampler);
	}


	// Draw Model
	for (uint32_t indexOffset = 0, i = 0; i < numMeshes; indexOffset += indexCount[i], ++i)
		context->DrawIndexed(indexCount[i], indexOffset, baseVertexOffset[i]);
}


//void Model::update(ID3D11DeviceContext *context) {

void Model::renderSimp(ID3D11DeviceContext *context) {



	// Validate Model before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !indexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set Model vertex and index buffers for IA
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


	// Draw Model
	for (uint32_t indexOffset = 0, i = 0; i < numMeshes; indexOffset += indexCount[i], ++i)
		context->DrawIndexed(indexCount[i], indexOffset, baseVertexOffset[i]);
}
