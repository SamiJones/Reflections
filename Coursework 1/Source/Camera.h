#pragma once
#include <DirectXMath.h>


class Camera
{
protected:
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR lookAt;
	DirectX::XMMATRIX projMat;

public:
	Camera();
	Camera(DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt);
	~Camera();

	// Accessor methods
	void setProjMatrix(DirectX::XMMATRIX setProjMat);
	void setLookAt(DirectX::XMVECTOR init_lookAt);
	void setPos(DirectX::XMVECTOR init_pos);
	void setUp(DirectX::XMVECTOR init_up);
	DirectX::XMMATRIX getViewMatrix();
	DirectX::XMMATRIX getProjMatrix();
	DirectX::XMVECTOR getPos();
	DirectX::XMVECTOR getLookAt();
	DirectX::XMVECTOR getUp();

	//this function is based on one written by Frank Luna in "3D Game Programming with DirectX11"
	void SetLens(float fovY, float aspect, float zn, float zf);
};

