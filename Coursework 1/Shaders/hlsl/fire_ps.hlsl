
//
// Fire effect 
//

// Ensure matrices are row-major
#pragma pack_matrix(row_major)


//-----------------------------------------------------------------
// Structures and resources
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Globals
//-----------------------------------------------------------------

cbuffer basicCBuffer : register(b0) {

	float4x4			worldViewProjMatrix;
	float4x4			projIMatrix;				// Correctly transform normals to world space
	float4x4			worldMatrix;
	float4				eyePos;
	float4				lightVec;					// w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
	float4				windDir;
	float				Timer;
};



//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D fireTexture : register(t0);
Texture2DMS  <float>depth: register(t1);
SamplerState linearSampler : register(s0);




//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {

	float4 posH  : SV_POSITION;  // in clip space
	float2 texCoord  : TEXCOORD0;
	float4 Z  : DEPTH;
	float alpha : ALPHA;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};

float unproject(float zBuf)
{
	float f = 1000.0;
	float n = 1.0;
	//return f*n / (f - zBuf*(f + n));
	return (2.0 * n) / (f + n - zBuf * (f - n));
}
//-----------------------------------------------------------------
// Pixel Shader - Simple
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket p) {
	
	FragmentOutputPacket outputFragment;

	float KSoften = 800; //Soften Constant
	float fragDepth  = p.Z.z / p.Z.w; // Finish perspective projection
	// Use load as we dont want filtering, use SV_POSITION as it is in framebuffer units not normalised texture coords.
	float zBufDepth = depth.Load(int4(p.posH.x, p.posH.y, 0, 0),0).r;

	// Linearise depth (0-1) as a perspective transformation results in a logarithmic depth values(0-1)
	zBufDepth = unproject(zBufDepth);
	fragDepth = unproject(fragDepth);

	// Soften the particle geometry intersection based on delta depth and KSoften Factor.
	float alphaSoft = saturate((zBufDepth - fragDepth)*KSoften);

	float4 col = fireTexture.Sample(linearSampler, p.texCoord);
	outputFragment.fragmentColour = float4(col.xyz, alphaSoft*p.alpha*(col.x + col.y + col.z) / 3);
	//outputFragment.fragmentColour = float4(zBufDepth, zBufDepth, zBufDepth, 1);
	return outputFragment;
}
