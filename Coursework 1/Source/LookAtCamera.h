#pragma once

#include <DirectXMath.h>
#include <Camera.h>

class LookAtCamera : public Camera {

private:




public:
	void* operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}



	//
	// Camera transformations
	//
	
	void rotateElevation(float t) {
		pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixRotationX(t));
	}
	void rotateOnYAxis(float t) {
		pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixRotationY(t));
	}
	void zoomCamera(float zoomFactor) {
		pos = DirectX::XMVector4Transform(pos, DirectX::XMMatrixScaling(zoomFactor, zoomFactor, zoomFactor));
	}


};

