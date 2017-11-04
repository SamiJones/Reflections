#pragma once
#include <Mesh.h>


class Grid : public Mesh {
protected:


	// Create the indices

	UINT width, height;
	UINT *indices;
	DXVertexExt *vertices;

	public:

	Grid(UINT width, UINT  height, ID3D11Device *device, Effect*_effect, ID3D11ShaderResourceView *tex_view, Material*_material);
	~Grid();

	UINT getWidth(){return width;};
	UINT getHeight(){ return height; };
	UINT getNumInd(){ return numInd; };

};