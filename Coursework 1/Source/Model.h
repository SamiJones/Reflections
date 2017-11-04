
//
// Model.h
//

// Version 1.  Encapsulate the mesh contents of a CGModel imported via CGImport3.  Currently supports obj, 3ds or gsf files.  md2, md3 and md5 (CGImport4) untested.  For version 1 a single texture and sampler interface are associated with the Model.


#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <DXBaseModel.h>
#include <Animation.h>
#include <string>
#include <vector>
#include <cstdint>

class Texture;
class Material;
class Effect;


class Model : public DXBaseModel {
	Animation *animation= nullptr;
	Material *material = nullptr;
	Effect *effect = nullptr;

	uint32_t							numMeshes = 0;
	std::vector<uint32_t>				indexCount;
	std::vector<uint32_t>				baseVertexOffset;
	
	ID3D11ShaderResourceView			*textureResourceView = nullptr;
	int Num_Textures;
	ID3D11ShaderResourceView			*textureResourceViewArray[8];
	ID3D11SamplerState					*sampler = nullptr;
	DirectX::XMMATRIX worldMatrix;
public:

	Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *tex_view, Material *_material);
	Model(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *_tex_view_array[], int _num_textures, Material *_material);

	
	~Model();
	DirectX::XMMATRIX update(double time){ if (animation != nullptr)worldMatrix= animation->update(time); return worldMatrix; };
	void load(ID3D11Device *device, Effect *_effect, const std::wstring& filename, ID3D11ShaderResourceView *tex_view, Material *_material);
	void update(ID3D11DeviceContext *context, double time);
	void render(ID3D11DeviceContext *context);
	void renderSimp(ID3D11DeviceContext *context);
	void setAnimation(Animation *newAnimation){ animation = newAnimation; };
};
