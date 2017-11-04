
//----------------------------
// Input / Output structures
//----------------------------
struct vertexInputPacket {

	float3				pos			: POSITION;
	float4				colour		: COLOR;
};


struct vertexOutputPacket {

	float4				colour		: COLOR;
	float2				texCoord	: TEXCOORD;
	float4				posH		: SV_POSITION;

};




//
// Vertex shader
//
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;

	outputVertex.colour = inputVertex.colour;
	outputVertex.texCoord = inputVertex.pos.xy*0.5+0.5;
	outputVertex.posH = float4(inputVertex.pos*0.9,1);//mul(float4(inputVertex.pos, 1.0), viewProjMatrix);

	return outputVertex;
}
