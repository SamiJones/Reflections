
// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	float4x4			viewProjMatrix;
	float4x4			worldITMatrix;				// Not used
	float4x4			worldMatrix;				// Not used
	float4x4			cameraViewMatrices[6];
	float4				eyePos;
	float4				lightVec;					// Not used
	float4				lightAmbient;				// Not used
	float4				lightDiffuse;				// Not used
	float4				lightSpecular;				// Not used
	float4				light2Vec; // w=1: Vec represents position, w=0: Vec  represents direction.
	float4				light2Ambient;
	float4				light2Diffuse;
	float4				light2Specular;
	float4				windDir;					// Not used
	float				Timer;
};

//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

struct VertexInputPacket {
	float3 pos : POSITION;   // in object space
	float3 vel :VELOCITY;   // in object space
	float3 data : DATA;   // x=age
};

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct VertexOutputPacket {

	float4 posH  : SV_POSITION;  // in clip space
	float2 texCoord  : TEXCOORD0;
	float4 Z  : DEPTH;
	float alpha : ALPHA;
};

//
// Geometry Shader
//

//Each vertex is expanded into a screen-aligned quad in view coordinate space.  
[maxvertexcount(4)]
void main(point VertexInputPacket inputVertex[1],
	inout TriangleStream<VertexOutputPacket> outputTriangleStream) {

	//tweakable parameters
	float gPartLife = 0.7;//seconds
	float gPartScale = 0.5;
	float gPartSpeed = 2;

	VertexOutputPacket		outputVertex;
	float age = inputVertex[0].data.x;
	float ptime = fmod(Timer + (age*gPartLife), gPartLife);
	float size = (gPartScale*ptime) + (gPartScale * 2);

	// Compute world matrix so that billboard faces the camera.
	float3 look = normalize(eyePos - inputVertex[0].pos);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);
	float2 posL[4];

	posL[0] = float2(1, 1);
	posL[1] = float2(1, -1);
	posL[2] = float2(-1, 1);
	posL[3] = float2(-1, -1);

	outputVertex.alpha = 1 - (ptime / gPartLife);

	for (int i = 0; i < 4; i++)
	{


		// Transform to world space.
		float3 pos = inputVertex[0].pos + (posL[i].x*right*size) + (posL[i].y*up*size * 2);
			pos += ptime*inputVertex[0].vel*gPartSpeed;

		// Transform to homogeneous clip space.
		outputVertex.posH = mul(float4(pos, 1.0f), viewProjMatrix);
		outputVertex.Z = outputVertex.posH;
		outputVertex.texCoord = float2((posL[i].x + 1)*0.5, (posL[i].y + 1)*0.5);
		outputTriangleStream.Append(outputVertex);
	}


}