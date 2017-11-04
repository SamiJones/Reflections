
// Ensure matrices are row-major
#pragma pack_matrix(row_major)



// Input / Output structures

struct InputVertexStructure {

	float4				pos				: SV_POSITION;
	float4				colour			: COLOR;
};

struct VertexOutputPacket {

	float4				colour			: COLOR;
	float4				posH			:SV_POSITION;
};


//
// Geometry Shader
//

//// Each triangle is output unchanged.  
//
//[maxvertexcount(3)]
//void main(triangle InputVertexStructure inputVertex[3],
//	inout TriangleStream<VertexOutputPacket> outputTriangleStream) {
//
//	VertexOutputPacket		outputVertex;
//	//top middle
//	outputVertex.colour = inputVertex[1].colour;
//	outputVertex.posH =  float4(inputVertex[1].pos.xyz,1);
//	outputTriangleStream.Append(outputVertex);
//
//	//bottom left
//	outputVertex.colour = inputVertex[0].colour;
//	outputVertex.posH = float4(inputVertex[0].pos.xyz, 1);
//	outputTriangleStream.Append(outputVertex);
//
//	//bottom right
//	outputVertex.colour = inputVertex[2].colour;
//	outputVertex.posH = float4(inputVertex[2].pos.xyz, 1);
//	outputTriangleStream.Append(outputVertex);
//
//
//}


 //Each triangle is expanded into a screen-aligned quad in view coordinate space.  

[maxvertexcount(4)]
void main(triangle InputVertexStructure inputVertex[3],
	inout TriangleStream<VertexOutputPacket> outputTriangleStream) {

	VertexOutputPacket		outputVertex;
	
	//top middle becomes top left
	outputVertex.colour = inputVertex[1].colour;
	outputVertex.posH =  float4(-0.9 ,inputVertex[1].pos.yz,1);
	outputTriangleStream.Append(outputVertex);

	//top right (new vertex)
	outputVertex.colour = float4(1, 1, 1, 1);
	outputVertex.posH = float4(0.9, 0.9, 1, 1);
	outputTriangleStream.Append(outputVertex);

   //bottom left
	outputVertex.colour = inputVertex[0].colour;
	outputVertex.posH = float4(inputVertex[0].pos.xyz,1);
	outputTriangleStream.Append(outputVertex);
    
	//bottom right
	outputVertex.colour = inputVertex[2].colour;
	outputVertex.posH = float4(inputVertex[2].pos.xyz, 1);
	outputTriangleStream.Append(outputVertex);

}

//// A quad is created at each point of the triangle
//// Remember to change the primitive topology from triangle list to point list when the triangle is drawn e.g.
////	// Set primitive topology for IA
////	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//[maxvertexcount(4)]
//void main(point InputVertexStructure inputVertex[1],
//	inout TriangleStream<VertexOutputPacket> outputTriangleStream) {
//
//	VertexOutputPacket		outputVertex;
//	
//	
//	//top left
//	outputVertex.colour = inputVertex[0].colour;
//	outputVertex.posH = float4(inputVertex[0].pos.xyz + float3(-0.1, 0.1, 0), 1);
//	outputTriangleStream.Append(outputVertex);
//	
//	//top right
//	outputVertex.colour = inputVertex[0].colour;
//	outputVertex.posH = float4(inputVertex[0].pos.xyz + float3(0.1, 0.1, 0), 1);
//	outputTriangleStream.Append(outputVertex);
//	
//	//bottom left
//	outputVertex.colour = inputVertex[0].colour;
//	outputVertex.posH = float4(inputVertex[0].pos.xyz + float3(-0.1, -0.1, 0), 1);
//	outputTriangleStream.Append(outputVertex);
//
//	//  bottom right
//	outputVertex.colour = inputVertex[0].colour;
//	outputVertex.posH = float4(inputVertex[0].pos.xyz + float3(0.1, -0.1, 0), 1);
//	outputTriangleStream.Append(outputVertex);
//}