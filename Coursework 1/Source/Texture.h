#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <d3d11_2.h>

class Texture
{
public:
// Direct3D scene textures and resource views
	ID3D11Texture2D							*texture = nullptr;
	ID3D11ShaderResourceView				*SRV = nullptr;
	ID3D11DepthStencilView					*DSV = nullptr;
	ID3D11RenderTargetView					*RTV = nullptr;
	Texture(ID3D11Device *device, const std::wstring& filename);
	~Texture();
};

