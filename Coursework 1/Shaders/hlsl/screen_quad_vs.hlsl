
//----------------------------
// Input / Output structures
//----------------------------
struct vertexInputPacket {

	float3				pos		: POSITION;
	float4				colour		: COLOR;
};


struct vertexOutputPacket {

	float2				texCoord	: TEXCOORD;
	float4				posH		: SV_POSITION;
};


//
// Vertex shader
//
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;

	outputVertex.posH = float4(inputVertex.pos, 1.0);
	outputVertex.texCoord = (inputVertex.pos + 1)*0.5;
	outputVertex.texCoord.y = 1 - outputVertex.texCoord.y;

	return outputVertex;
}
