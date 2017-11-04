
//
// Pass-through vertex shader for the snow particle system demo.  This is used for both the update and render pipeline configurations.  Billboarding will be done in eye / view coordinate space, but there is no loss of efficiency in converting particles to eye / view coordinates in the geometry shader since the input primitive is only points - there is no duplication of vertex processing in this example.
//


// Input / Output structures

struct ThroughVertexPacket {

	float3				pos				: POSITION;
	float3				velocity		: VELOCITY;
	float				weight : WEIGHT;
	float				age : AGE;
	float				theta : ORIENTATION;
	float				angularVelocity : ANGULARVELOCITY;
	uint				isaGenerator	: GENERATOR;

	// Determine which texture will be used [0, 7]
	uint				flakeType		: FLAKETYPE;
};



//
// Vertex Shader
//

ThroughVertexPacket main(ThroughVertexPacket inputVertex) {

	return inputVertex;
}
