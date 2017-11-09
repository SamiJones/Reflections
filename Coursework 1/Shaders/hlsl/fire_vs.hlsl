
//
// Fire effect
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)



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

	float3 pos  : POSITION;  // unchanged
	float3 vel :VELOCITY;   // unchanged
	float3 data : DATA;// unchanged

};
//-----------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------
vertexOutputPacket main(vertexInputPacket vin) {


	vertexOutputPacket vout = (vertexOutputPacket)0;
	vout.pos = vin.pos;
	vout.data.x = vin.data.x;
	vout.vel = vin.vel;

	return vout;

}