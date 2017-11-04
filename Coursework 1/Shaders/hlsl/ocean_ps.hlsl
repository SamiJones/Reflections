
//
// Basic Water Simulation - Modified from NVidia Ocean.fx
//
// NVIDIA Comments :
/****************************************************************************
Simple ocean shader with animated bump map and geometric waves
Based partly on "Effective Water Simulation From Physical Models", GPU Gems
******************************************************************************/

// Ensure matrices are row-major
#pragma pack_matrix(row_major)

//-----------------------------------------------------------------
// Structures and resources
//-----------------------------------------------------------------

// Textures

// Assumes normal map bound to texture t0 
Texture2D gNormMap : register(t0);
// Cube map bound to texture t1
TextureCube gCubeMap : register(t1);
// Normal linear sampler bound to sampler s0
TextureCube gCubeMapDynamic : register(t2);
// Normal linear sampler bound to sampler s0
SamplerState gNormalLinearSam : register(s0);
// Tri-linear sampler bound to sampler s1
SamplerState gTriLinearSam : register(s1);

// Globals

cbuffer basicCBuffer : register(b0) {

	float4x4			worldViewProjMatrix;
	float4x4			worldITMatrix; // Correctly transform normals to world space
	float4x4			worldMatrix;
	float4				eyePos;
	float4				lightVec; // w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
	float4				windDir;
	float				Timer;
	float				grassHeight;
};

//-----------------------------------------------------------------
// Input / Output structures
//-----------------------------------------------------------------

// Input fragment - this is the per-fragment packet interpolated by the rasteriser stage
struct FragmentInputPacket {
	// Eye in world coords
	float3				eyeDir		: POSITION;		// Direction to eye from vertex in world coords

	float3				T2WXf1		: T2WMATRIX1;	// first row of the 3x3 transform from tangent to cube space
	float3				T2WXf2		: T2WMATRIX2;	// second row of the 3x3 transform from tangent to cube space
	float3				T2WXf3		: T2WMATRIX3;	// third row of the 3x3 transform from tangent to cube space
	// Normal map UV  coords
	float2				bumpUV0		: TEXCOORD1;	// Small ripples
	float2				bumpUV1		: TEXCOORD2;	// Medium ripples
	float2				bumpUV2		: TEXCOORD3;	// Large ripples

	float4				posH		: SV_POSITION;  // in clip space
	};

struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};

//-----------------------------------------------------------------
// Pixel Shader - Lighting 
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket IN) { 

	FragmentOutputPacket outputFragment;
	
	///////// TWEAKABLE PARAMETERS //////////////////
	float FresnelBias =		0.3;
	float FresnelExp =		4.0;
	float3 DeepColor =		{ 0.0f, 0.0f, 0.1f };
	float3 ShallowColor =	{ 0.0f, 0.5f, 0.5f };
	float3 ReflTint =		{ 1.0f, 1.0f, 1.0f };
	float Kr =				1.0f;
	float KWater =			1.0f;

	// Sum normal maps
	float4 t0 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV0)*2.0 - 1.0;
		float4 t1 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV1)*2.0 - 1.0;
		float4 t2 = gNormMap.Sample(gNormalLinearSam, IN.bumpUV2)*2.0 - 1.0;
		float3 Nt = t0.xyz + t1.xyz + t2.xyz;

	// Transform normals from texture space to world coordinates
	// Add Code Here (Reconstruct float3X3 tangent to world matrix)


	float3x3 m; // Tangent to world matrix

	// Transform normal to world coordinates
	// Modify Code Here (Transform normal to world coordinates)
	m[0] = IN.T2WXf1;
	m[1] = IN.T2WXf2;
	m[2] = IN.T2WXf3;
	float3 Nw = mul(m, Nt);


	float3 Nn = normalize(Nw);
	//Nn = float3(0, 1, 0);

	// Reflection
	float3 Vn = normalize(IN.eyeDir);
		float3 R = reflect(-Vn, Nn);
		float4 reflection = gCubeMapDynamic.Sample(gTriLinearSam, R);

		reflection.xyz = lerp(reflection.xyz, gCubeMap.Sample(gTriLinearSam, R).xyz,  reflection.a);

	
	// Fresnel
	float facing = 1.0 - max(dot(Vn, Nn), 0);
	// Modify Code Here (Complete Fresnel calculation)
	float3 result = reflection;

	outputFragment.fragmentColour = float4(result.rgb, facing+0.2);

	return outputFragment;
}
