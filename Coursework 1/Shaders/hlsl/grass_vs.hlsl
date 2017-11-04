
//
// Grass effect - Modified a fur technique
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	float4x4			worldViewProjMatrix;
	float4x4			worldITMatrix;				// Correctly transform normals to world space
	float4x4			worldMatrix;
	float4				eyePos;
	float4				lightVec;					// w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
	float4				windDir;
	float				Timer;
	float				grassHeight;
};




//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {

	float3				pos			: POSITION;
	float3				normal		: NORMAL;
	float4				matDiffuse	: DIFFUSE; // a represents alpha.
	float4				matSpecular	: SPECULAR;  // a represents specular power. 
	float2				texCoord	: TEXCOORD;
};


struct vertexOutputPacket {


	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float4				matDiffuse		: DIFFUSE;
	float4				matSpecular		: SPECULAR;
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};

Texture2D heightTexture : register(t0);
Texture2D normalTexture : register(t1);

//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;
	float3 pos = inputVertex.pos;
	//pos.y = heightTexture.Load(int4(inputVertex.texCoord.x * 1024, inputVertex.texCoord.y * 1024, 0, 0)).r;

	// Lighting is calculated in world space.
	outputVertex.posW = mul(float4(pos, 1.0f), worldMatrix).xyz;
	
	// Transform normals to world space with gWorldIT.
	outputVertex.normalW = mul(float4(inputVertex.normal, 1.0f), worldITMatrix).xyz;
	//outputVertex.normalW = (normalTexture.Load(int3(inputVertex.texCoord.x * 1024, inputVertex.texCoord.y * 1024, 0)).xyz*2-1);
	
	// Pass through material properties
	outputVertex.matDiffuse = inputVertex.matDiffuse;
	outputVertex.matSpecular = inputVertex.matSpecular;
	// .. and texture coordinates.
	outputVertex.texCoord = inputVertex.texCoord;
	
	
	// Finally transform/project pos to screen/clip space posH
	pos.y += grassHeight*4;
	float k = pow(grassHeight*200, 3);
	float3 gWindDir = float3(sin(Timer)*0.05, 0, 0);
	pos = pos + gWindDir*k;
	outputVertex.posH = mul(float4(pos, 1.0), worldViewProjMatrix);

	return outputVertex;
}
