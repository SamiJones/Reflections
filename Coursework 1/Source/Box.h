#pragma once

#include <GUObject.h>
class Effect;


class Box : public GUObject {

	ID3D11Buffer					*vertexBuffer = nullptr;
	ID3D11Buffer					*indexBuffer = nullptr;
	Effect							*effect = nullptr;

	// Augment box with texture view
	ID3D11ShaderResourceView			*textureResourceView = nullptr;
	ID3D11SamplerState					*sampler = nullptr;
public:

	Box(ID3D11Device *device, Effect *_effect, ID3D11ShaderResourceView *tex_view);
	~Box();
	void setTexture(ID3D11ShaderResourceView *tex_view);
	void render(ID3D11DeviceContext *context);
};