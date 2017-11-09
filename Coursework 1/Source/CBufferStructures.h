#pragma once
using namespace DirectX;
using namespace DirectX::PackedVector;
// CBuffer struct
// Use 16byte aligned so can use optimised XMMathFunctions instead of setting _XM_NO_INNTRINSICS_ define when compiling for x86
__declspec(align(16)) struct CBufferExt  {
	DirectX::XMMATRIX						WVPMatrix;
	DirectX::XMMATRIX						worldITMatrix; // Correctly transform normals to world space
	DirectX::XMMATRIX						worldMatrix;
	DirectX::XMMATRIX						cameraViewMatrices[6];
	DirectX::XMFLOAT4						eyePos;
	DirectX::XMFLOAT4						lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	DirectX::XMFLOAT4						lightAmbient;
	DirectX::XMFLOAT4						lightDiffuse;
	DirectX::XMFLOAT4						lightSpecular;
	DirectX::XMFLOAT4						light2Vec; // w=1: Vec represents position, w=0: Vec  represents direction.
	DirectX::XMFLOAT4						light2Ambient;
	DirectX::XMFLOAT4						light2Diffuse;
	DirectX::XMFLOAT4						light2Specular;

	// from terrain tutorial
	DirectX::XMFLOAT4						windDir;
	// Simple single light source properties
	FLOAT									Timer;
	// from terrain tutorial
	FLOAT									grassHeight;
	//// from terrain tutorial
	//FLOAT									reflectionPass;
	// from particle system tutorial
	//FLOAT									fireHeight;

};

__declspec(align(16)) struct camStruct {
	DirectX::XMMATRIX						WVPMatrix;
	DirectX::XMMATRIX						worldITMatrix; // Correctly transform normals to world space
	DirectX::XMMATRIX						worldMatrix;
	DirectX::XMFLOAT4						eyePos;
};
__declspec(align(16)) struct lightStruct {
	DirectX::XMFLOAT4						lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	DirectX::XMFLOAT4						lightAmbient;
	DirectX::XMFLOAT4						lightDiffuse;
	DirectX::XMFLOAT4						lightSpecular;
};

__declspec(align(16)) struct projMatrixStruct  {
	DirectX::XMMATRIX						projMatrix;
};
// from terrain tutorial
__declspec(align(16)) struct worldMatrixStruct  {
	DirectX::XMMATRIX						worldMatrix;
};

struct MaterialStruct
{
	XMCOLOR emissive;
	XMCOLOR ambient;
	XMCOLOR diffuse;
	XMCOLOR specular;
};
struct LightStruct
{
	XMFLOAT4 attenuation; // Constant, Linear, Quadratic Attenuation
	XMFLOAT4 vector; //w = 1: Vec represents position, w = 0 : Vec  represents direction.
	XMCOLOR ambient;
	XMCOLOR diffuse;
	XMCOLOR specular;
};





