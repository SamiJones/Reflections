#include "stdafx.h"
#include "Camera.h"


Camera::Camera() {
	pos = DirectX::XMVectorSet(0, 0, -10, 1.0f);
	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lookAt = DirectX::XMVectorZero();
}
Camera::Camera(DirectX::XMVECTOR init_pos, DirectX::XMVECTOR init_up, DirectX::XMVECTOR init_lookAt) {
	pos = init_pos;
	up = init_up;
	lookAt = init_lookAt;
}
Camera::~Camera()
{
}

//
// Accessor methods
//
void Camera::setProjMatrix(DirectX::XMMATRIX setProjMat) {
	projMat = setProjMat;
}
void Camera::setLookAt(DirectX::XMVECTOR init_lookAt) {
	lookAt = init_lookAt;
}
void Camera::setPos(DirectX::XMVECTOR init_pos) {
	pos = init_pos;
}
void Camera::setUp(DirectX::XMVECTOR init_up) {
	up = init_up;
}
DirectX::XMMATRIX Camera::getViewMatrix() {
	return 		DirectX::XMMatrixLookAtLH(pos, lookAt, up);
}
DirectX::XMMATRIX Camera::getProjMatrix() {
	return 		projMat;
}
DirectX::XMVECTOR Camera::getPos() {
	return pos;
}
DirectX::XMVECTOR Camera::getLookAt() {
	return lookAt;
}
DirectX::XMVECTOR Camera::getUp() {
	return up;
}