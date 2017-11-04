
//
// Fire effect
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	float4x4			viewProjMatrix;
	float4x4			worldITMatrix;				// Not used
	float4x4			worldMatrix;				// Not used
	float4				eyePos;
	float4				lightVec;					// Not used
	float4				lightAmbient;				// Not used
	float4				lightDiffuse;				// Not used
	float4				lightSpecular;				// Not used
	float4				windDir;					// Not used
	float				Timer;
};




//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {

	float3 pos : POSITION;   // in object space
	float3 posL : LPOS;   // in object space
	float3 vel :VELOCITY;   // in object space
	float3 data : DATA;
};


struct vertexOutputPacket {

	float4 posH  : SV_POSITION;  // in clip space
	float2 texCoord  : TEXCOORD0;
	float4 Z : DEPTH;
	float alpha : ALPHA;
};
//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket vin) {

	float gPartLife = 0.7;//seconds
	float gPartScale = 0.2;
	float gPartSpeed = 2;

	vertexOutputPacket vout = (vertexOutputPacket)0;

	float age = vin.data.x;
	float ptime = fmod(Timer + (age*gPartLife), gPartLife);
	float size = (gPartScale*ptime) + (gPartScale * 2);
	vout.alpha = 1 - (ptime / gPartLife);

	// Compute world matrix so that billboard faces the camera.
	float3 look = normalize(eyePos - vin.pos);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);

	// Transform to world space.
	float3 pos = vin.pos + (vin.posL.x*right*size) + (vin.posL.y*up*size * 2);

	pos += ptime*vin.vel*gPartSpeed;

	// Transform to homogeneous clip space.
	vout.posH = mul(float4(pos, 1.0f), viewProjMatrix);
	vout.Z = vout.posH;
	vout.texCoord = float2((vin.posL.x + 1)*0.5, (vin.posL.y + 1)*0.5);
	return vout;

}
