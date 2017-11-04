
//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	row_major	float4x4	worldViewProjMatrix;
	row_major	float4x4	worldITMatrix;
	float3		lightDir;

};


//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------
struct vertexInputPacket {

	float3				pos			: POSITION;
	float3				normal		: NORMAL;
	float4				matDiffuse	: DIFFUSE;
	float4				matSpecular	: SPECULAR;
	float2				texCoord	: TEXCOORD;
};


struct vertexOutputPacket {

	float4				colour		: COLOR;
	float4				posH		: SV_POSITION;
};


//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;

	// Lighting is calculated in world space.
	// Transform normals to world space with gWorldIT.
	float3 norm = normalize(mul(float4(inputVertex.normal, 1.0f), worldITMatrix).xyz);
	// A basic diffuse light
	// outputVertex.colour = inputVertex.matDiffuse;
	outputVertex.colour = inputVertex.matDiffuse*saturate(dot(norm, normalize(-lightDir)));


	// outputVertex.posH = float4(inputVertex.pos, 1.0);
	outputVertex.posH = mul(float4(inputVertex.pos, 1.0), worldViewProjMatrix);

	return outputVertex;
}
