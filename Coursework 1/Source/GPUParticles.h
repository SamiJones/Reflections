#pragma once
#include "DXVertexParticle.h"
#include "Particles.h"



class GPUParticles : public Particles{


public:

	GPUParticles(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view, Material *_material);
	void render(ID3D11DeviceContext *context);
};