
#include <stdafx.h>
#include <Grid.h>
#include <DXVertexExt.h>
#include <Material.h>
using namespace std;
using namespace DirectX;




Grid::Grid(UINT widthl, UINT heightl, ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material*_material) :Mesh(device,_effect,tex_view,_material){// XMCOLOR in_Diffuse, XMCOLOR in_Specular) {
	

	width = widthl;
	height = heightl;
	numInd = ((width - 1) * 2 * 3)*(height - 1);
	vertices = (DXVertexExt*)malloc(sizeof(DXVertexExt)*width*height);
	indices = (UINT*)malloc(sizeof(UINT)*numInd);
	
	try
	{

		// Setup grid vertex buffer

		//INITIALISE Verticies
		for (int i = 0; i<height; i++)
		{
			for (int j = 0; j<width; j++)
			{

				vertices[(i*width) + j].pos.x = j;
				vertices[(i*width) + j].pos.z = i ;
				vertices[(i*width) + j].pos.y = 0;
				vertices[(i*width) + j].normal=XMFLOAT3(0, 1, 0);
				vertices[(i*width) + j].matDiffuse = material->getColour()->diffuse;
				vertices[(i*width) + j].matSpecular = material->getColour()->specular;
				vertices[(i*width) + j].texCoord.x = (float)j / width;
				vertices[(i*width) + j].texCoord.y = (float)i / height;
			}
		}



		// Setup vertex buffer
		D3D11_BUFFER_DESC vertexDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
		ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.ByteWidth = sizeof(DXVertexExt) * width*height;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexData.pSysMem = vertices;

		HRESULT hr = device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");


		for (int i = 0; i<height - 1; i++)
		{
			for (int j = 0; j<width - 1; j++)
			{
				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 0] = (i*width) + j;
				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 2] = (i*width) + j + 1;
				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 1] = ((i + 1)*width) + j;

				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 3] = (i*width) + j + 1;
				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 5] = ((i + 1)*width) + j + 1;;
				indices[(i*(width - 1) * 2 * 3) + (j * 2 * 3) + 4] = ((i + 1)*width) + j;
			}
		}


		D3D11_BUFFER_DESC indexDesc;
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.ByteWidth = sizeof(UINT)* numInd;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.CPUAccessFlags = 0;
		indexDesc.MiscFlags = 0;
		indexDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices;
		
		hr = device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);
		
		if (!SUCCEEDED(hr))
			throw exception("Vertex buffer cannot be created");

	}
	catch (exception& e)
	{
		cout << "Grid object could not be instantiated due to:\n";
		cout << e.what() << endl;

		if (vertexBuffer)
			vertexBuffer->Release();

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
	}
}



Grid::~Grid() {

	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();

	if (textureResourceView)
		textureResourceView->Release();

	if (linearSampler)
		linearSampler->Release();
}


