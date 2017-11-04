
// Basic texture copy pixel shader


// input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct fragmentInputPacket {

	float2				texCoord	: TEXCOORD;
	float4				posH		: SV_POSITION;
};


struct fragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
	float			fragmentDepth : SV_DEPTH;
};

//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2DMS  <float>depthTexture: register(t0);
SamplerState linearSampler : register(s0);

fragmentOutputPacket main(fragmentInputPacket inputFragment) {

	fragmentOutputPacket outputFragment;

	//float4 colour = copyTexture.Sample(linearSampler, inputFragment.texCoord);
	float zBufDepth = depthTexture.Load(int4(inputFragment.texCoord.x * 600, inputFragment.texCoord.y * 600, 0, 0), 0).r;
	outputFragment.fragmentColour = float4(0, 0, 0, 0.1);// colour;// float4(colour.rgb, (colour.r + colour.g + colour.b) / 3);
	outputFragment.fragmentDepth = zBufDepth;
	return outputFragment;
}
