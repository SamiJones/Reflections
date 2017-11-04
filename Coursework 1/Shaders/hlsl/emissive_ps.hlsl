
//
// Simple emisive shader
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)


//-----------------------------------------------------------------
// Structures and resources
//-----------------------------------------------------------------



//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D myTexture : register(t0);
SamplerState linearSampler : register(s0);




//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {

	// Vertex in world coords
	float3				posW			: POSITION;
	// Normal in world coords
	float3				normalW			: NORMAL;
	float4				matDiffuse		: DIFFUSE; // a represents alpha.
	float4				matSpecular		: SPECULAR; // a represents specular power. 
	float2				texCoord		: TEXCOORD;
	float4				posH			: SV_POSITION;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};


//-----------------------------------------------------------------
// Pixel Shader - Lighting 
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket v) {

	FragmentOutputPacket outputFragment;


		float4 baseColour = v.matDiffuse;

		baseColour = baseColour * myTexture.Sample(linearSampler, v.texCoord);


		outputFragment.fragmentColour = float4(baseColour.xyz , 0.1);
		//outputFragment.fragmentColour = float4(1,1,1, 1);
	return outputFragment;

}
