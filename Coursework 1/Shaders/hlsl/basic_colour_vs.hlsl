
//----------------------------
// Input / Output structures
//----------------------------
struct vertexInputPacket {

	float3				pos			: POSITION;
	float4				colour		: COLOR;
};


struct vertexOutputPacket {
	float4				posH			: SV_POSITION;
	float4				colour		: COLOR;

};


//
// Vertex shader
//
vertexOutputPacket main(vertexInputPacket inputVertex) {

	vertexOutputPacket outputVertex;

	outputVertex.colour = inputVertex.colour;
	outputVertex.posH = float4(inputVertex.pos*0.9,1);//mul(float4(inputVertex.pos, 1.0), viewProjMatrix);

	return outputVertex;
}
