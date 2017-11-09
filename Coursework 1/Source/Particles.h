#pragma once
#include "DXVertexParticle.h"
#include <GUObject.h>
#define N_PART 100
#define N_VERT N_PART*4
#define N_P_IND N_PART*6


class Texture;
class Material;
class Effect;



class Particles : public GUObject {

protected:

	Material *material = nullptr;
	Effect *effect = nullptr;


	// Create Particle vertex buffer
	DXVertexParticle vertices[N_VERT];

	// Create the indices
	UINT indices[N_P_IND];

	ID3D11Buffer					*vertexBuffer = nullptr;
	ID3D11Buffer					*indexBuffer = nullptr;
	ID3D11InputLayout				*inputLayout = nullptr;
	// Augment particles with texture view
	ID3D11ShaderResourceView			*textureResourceView = nullptr;
	ID3D11SamplerState				*linearSampler = nullptr;

public:
	Particles();
	Particles(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material);
	~Particles();
	void setTexture(ID3D11ShaderResourceView *tex_view);
	void render(ID3D11DeviceContext *context);
};