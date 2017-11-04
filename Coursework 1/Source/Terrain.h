#pragma once
#include "Grid.h"

class Effect;
class Material;


class Terrain : public Grid
{
public:
	Terrain(UINT widthl, UINT heightl, ID3D11DeviceContext *context, ID3D11Device *device, Effect *_effect,
		ID3D11ShaderResourceView *tex_view, Material *_material, ID3D11Texture2D *tex_height, ID3D11Texture2D *tex_normal);

	float CalculateYValue(float x, float z);
	void Terrain::render(ID3D11DeviceContext *context);
	~Terrain();
};

