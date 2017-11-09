
// Ensure matrices are row-major
#pragma pack_matrix(row_major)

cbuffer basicCBuffer : register(b0) {
	float4x4			worldViewProjMatrix;
	float4x4			worldITMatrix; // Correctly transform normals to world space
	float4x4			worldMatrix;
	float4x4			cameraViewMatrices[6];
	float4				eyePos;
	float4				lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
	float4				light2Vec; // w=1: Vec represents position, w=0: Vec  represents direction.
	float4				light2Ambient;
	float4				light2Diffuse;
	float4				light2Specular;
	float4				windDir;
	float				Timer;
	float				grassHeight;
}

//input to cube map geometry shader
struct GS_CUBEMAP_IN
{
	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float4				matDiffuse		: DIFFUSE;
	float4				matSpecular		: SPECULAR;
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};

//output from cube map geometry shader
struct PS_CUBEMAP_IN
{
	float3				posW			: POSITION; //set
	// Normal in world coords
	float3				normalW			: NORMAL; //set
	float4				matDiffuse		: DIFFUSE; // a represents alpha. //set
	float4				matSpecular		: SPECULAR; // a represents specular power.  //set
	float2				Tex				: TEXCOORD; //Texture coord //set
	float4				Pos				: SV_POSITION; //Projection coord //set

	uint				RTIndex			: SV_RenderTargetArrayIndex; //set
};

[maxvertexcount(18)]
void main(triangle GS_CUBEMAP_IN input[3],
	inout TriangleStream<PS_CUBEMAP_IN> CubeMapStream)
{
	// For each triangle
	for (int f = 0; f < 6; ++f)
	{
		// Compute screen coordinates
		PS_CUBEMAP_IN output;
		// Assign the ith triangle to the ith render target.
		output.RTIndex = f;
		// For each vertex in the triangle
		for (int v = 0; v < 3; v++)
		{
			// Transform to the view space of the ith cube face.
			output.Pos = mul(float4(input[v].posW, 1), cameraViewMatrices[f]);
			// Transform to homogeneous clip space.
			output.Pos = mul(output.Pos, worldViewProjMatrix);
			output.Tex = input[v].texCoord;
			
			output.matDiffuse = input[v].matDiffuse;
			output.matSpecular = input[v].matSpecular;
			output.posW = mul(float4(input[v].posW, 1.0f), worldMatrix).xyz;
			// Transform normals to world space with gWorldIT.
			output.normalW = mul(float4(input[v].normalW, 1.0f), worldITMatrix).xyz;

			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}
