#include "stdafx.h"
#include "Terrain.h"
#include "Effect.h"
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

Terrain::Terrain(UINT widthl, UINT heightl, ID3D11DeviceContext *context, ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material, ID3D11Texture2D *tex_height, ID3D11Texture2D *tex_normal) : Grid(widthl, heightl, device, _effect,  tex_view, _material)
{

	if (vertexBuffer)
		vertexBuffer->Release();
	ID3D11Texture2D* grassHeightStage = 0;
	ID3D11Texture2D* grassNormalStage = 0;
	D3D11_TEXTURE2D_DESC heightDesc;
	tex_height->GetDesc(&heightDesc);
	UINT texWidth = heightDesc.Width;
	UINT texHeight = heightDesc.Height;

	// CPU Access buffer
	D3D11_TEXTURE2D_DESC StagedDesc = {
		texWidth,//UINT Width;
		texHeight,//UINT Height;
		1,//UINT MipLevels;
		1,//UINT ArraySize;
		DXGI_FORMAT_R8G8B8A8_UNORM,//DXGI_FORMAT Format;
		1, 0,//DXGI_SAMPLE_DESC SampleDesc;
		D3D11_USAGE_STAGING,//D3D11_USAGE Usage;
		0,//UINT BindFlags;
		D3D11_CPU_ACCESS_READ,//UINT CPUAccessFlags;
		0//UINT MiscFlags;
	};
	device->CreateTexture2D(&StagedDesc, NULL, &grassHeightStage);
	device->CreateTexture2D(&StagedDesc, NULL, &grassNormalStage);

	context->CopyResource(grassHeightStage,static_cast<ID3D11Resource*>( tex_height));
	context->CopyResource(grassNormalStage, static_cast<ID3D11Resource*>(tex_normal));
	//context->CopyResource(grassNormalStage, tex_normal);


	// Lock the memory
	D3D11_MAPPED_SUBRESOURCE MappingDesc;
	D3D11_MAPPED_SUBRESOURCE MappingDescNorms;
	context->Map(grassHeightStage, 0, D3D11_MAP_READ, 0, &MappingDesc);
	context->Map(grassNormalStage, 0, D3D11_MAP_READ, 0, &MappingDescNorms);
	// Get the data
	if (MappingDesc.pData == NULL) {
		::MessageBox(NULL, L"DrawSurface_GetPixelColor: Could not read the pixel color because the mapped subresource returned NULL.", L"Error!", NULL);
	}
	else {

		if (MappingDescNorms.pData == NULL) 
			::MessageBox(NULL, L"Normals: DrawSurface_GetPixelColor: Could not read the pixel color because the mapped subresource returned NULL.", L"Error!", NULL);


		UINT8* Result;
		Result = (UINT8*)MappingDesc.pData;
		UINT8* ResultNorms;
		ResultNorms = (UINT8*)MappingDescNorms.pData;
		//INITIALISE Verticies
		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{
				int xi = (int)(vertices[(i*width) + j].texCoord.x*texWidth);
				int zi = (int)(vertices[(i*width) + j].texCoord.y*texHeight);
				vertices[(i*width) + j].pos.y = (((float)Result[xi * texWidth * 4 + zi * 4]) / 255.0) * 5;

				vertices[(i*width) + j].normal.z = (((float)ResultNorms[xi * texWidth * 4 + zi * 4]) / 255.0)*2.0-1.0;
				vertices[(i*width) + j].normal.x = (((float)ResultNorms[xi * texWidth * 4 + zi * 4 + 1]) / 255.0)*2.0 - 1.0;
				vertices[(i*width) + j].normal.y = ((((float)ResultNorms[xi * texWidth * 4 + zi * 4 + 2]) / 255.0)*2.0 - 1.0) * 2;
			}
		}


		//Copy the matrices into the  vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexExt)* width*height;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);
	}
	// Unlock the memory
	context->Unmap(grassHeightStage, 0);

}
float Terrain::CalculateYValue(float x, float z)
{
	x = x*width;
	z = z*height;
	//	check range
	if (x<0 || x>this->width || z<0 || z>this->height)
		return 0;

	//	Retrieve the points for the current quad we are in
	float fTopLeft = vertices[(int)x + (((int)z + 1) * this->width)].pos.y;
	float fTopRight = vertices[((int)x) + (((int)z + 1) * this->width) + 1].pos.y;
	float fBottomLeft = vertices[((int)x) + (((int)z) * this->width)].pos.y;
	float fBottomRight = vertices[((int)x) + (((int)z) * this->width) + 1].pos.y;

	float finalHeight = 0;	//	what we are trying to find
	//	fraction parts of x and z
	float frac_x = x - ((int)x);
	float frac_z = z - ((int)z);


	//	What triangle are we in? (top right or bottom left)
	if ((frac_x*frac_x + frac_z*frac_z) < ((1 - frac_x)*(1 - frac_x) + (1 - frac_z)*(1 - frac_z)))
	{
		//in bottom left triangle
		float dX = fBottomRight - fBottomLeft;	//	how the height changes with respect to x
		float dZ = fTopLeft - fBottomLeft;		//	how the height changes with respect to z
		//	calculate our final height
		finalHeight = dX * frac_x + dZ * frac_z + fBottomLeft;
	}
	else
	{
		//in top right triangle
		float dX = fTopLeft - fTopRight;	//	how the height changes with respect to x
		float dZ = fBottomRight - fTopRight;//	how the height changes with respect to z
		//	calculate our final height
		finalHeight = dX * (1 - frac_x) + dZ * (1 - frac_z) + fTopRight;
	}
	//finalHeight = vertices[(int)round((x*width) + z)].pos.y;
	//	return our calculated height
	return ((float)finalHeight);
}

Terrain::~Terrain()
{
}


void Terrain::render(ID3D11DeviceContext *context) {

	//effect->bindPipeline(context);

	// set shader reflection map shaders for shark
	context->VSSetShader(effect->getVertexShader(), 0, 0);
	context->PSSetShader(effect->getPixelShader(), 0, 0);


	// Validate DXModel before rendering (see notes in constructor)
	if (!context || !vertexBuffer || !indexBuffer || !effect)
		return;

	// Set vertex layout
	context->IASetInputLayout(effect->getVSInputLayout());

	// Set DXModel vertex and index buffers for IA
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer };
	UINT vertexStrides[] = { sizeof(DXVertexExt) };
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

	// Draw Mesh
	context->DrawIndexed(numInd, 0, 0);
}