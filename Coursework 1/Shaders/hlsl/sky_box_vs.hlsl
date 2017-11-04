

// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	float4x4			worldViewProjMatrix;

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



	float3				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};


//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;

	outputVertex.texCoord = inputVertex.pos;
	
	// Transform/project pos to screen/clip space posH ensuring that pos.z=1(far clipping plane)
	outputVertex.posH = mul(float4(inputVertex.pos, 1.0), worldViewProjMatrix).xyww;
	//outputVertex.posH.z=200.0;

	return outputVertex;
}
