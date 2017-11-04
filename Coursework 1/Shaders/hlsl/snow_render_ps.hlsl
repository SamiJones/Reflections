
//
// Basic texture-lookup pixel shader for rendering the snow particle system
//


// Resources

Texture2DArray			snowflakeArray : register(t1);
SamplerState			linearSampler : register(s0);


// Input fragment
struct FragmentInputPacket {

	float4				colour			: COLOR;
	float2				texCoord		: TEXCOORD;
	uint				id				: FLAKETYPE;
	float				lDepth			: LINEAR_DEPTH;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};



FragmentOutputPacket main(FragmentInputPacket inputFragment) {

	FragmentOutputPacket outputFragment;

	outputFragment.fragmentColour = snowflakeArray.Sample(linearSampler, float3(inputFragment.texCoord, inputFragment.id % 8));

	return outputFragment;
}
