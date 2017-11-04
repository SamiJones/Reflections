#pragma once

#include <DirectXMath.h>
#include <Camera.h>


class FirstPersonCamera : public Camera
{


private:

	DirectX::XMVECTOR dir;
	float ang;

public:

	FirstPersonCamera();
	FirstPersonCamera(DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt);
	~FirstPersonCamera();

	void move(float d);
	void turn(float d);
	void elevate(float d);

	//overrides
	DirectX::XMMATRIX FirstPersonCamera::getViewMatrix();

};
