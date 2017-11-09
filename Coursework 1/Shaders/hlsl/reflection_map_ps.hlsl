
//
// Model a simple light
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

};



//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D diffMap : register(t0);
TextureCube envMap : register(t1);
Texture2D specMap : register(t2);
TextureCube envMapDynamic : register(t3);
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
	float2				Tex				: TEXCOORD; //Texture coord
	float4				Pos				: SV_POSITION; //Projection coord

	uint				RTIndex			: SV_RenderTargetArrayIndex;
};


struct FragmentOutputPacket {

	float4				fragmentColour : SV_TARGET;
};


//-----------------------------------------------------------------
// Pixel Shader - Lighting 
//-----------------------------------------------------------------

FragmentOutputPacket main(FragmentInputPacket v) {

	FragmentOutputPacket outputFragment;


	///////// PARAMETERS Could be added to CBUFFER //////////////////
	float FresnelBias = 0.1;//0.3;
	float FresnelExp = 0.5;//4;
	float useSpecMap = 1.0;
	float useDiffMap = 1.0;

	float3 N = normalize(v.normalW);
	float4 baseColour = v.matDiffuse;

	if (useDiffMap>0.0)
		baseColour *= diffMap.Sample(linearSampler, v.Tex);

	//Initialise returned colour to ambient component
	float3 finalColour = baseColour.xyz * (lightAmbient.xyz + light2Ambient.xyz);

	// Calculate the lambertian term (essentially the brightness of the surface point based on the dot product of the normal vector with the vector pointing from v to the light source's location)
	float3 lightDir = -lightVec.xyz; // Directional light
	if (lightVec.w == 1.0)
		lightDir = lightVec.xyz - v.posW; // Positional light
	lightDir = normalize(lightDir);

	float3 light2Dir = -light2Vec.xyz; // Directional light
		if (light2Vec.w == 1.0)
			light2Dir = light2Vec.xyz - v.posW; // Positional light
	light2Dir = normalize(light2Dir);

	// Add diffuse light 
	finalColour += max(dot(lightDir, N), 0.0f) * baseColour.xyz * lightDiffuse;
	finalColour += max(dot(light2Dir, N), 0.0f) * baseColour.xyz * light2Diffuse;

	// Add reflection
	float specFactor = v.matSpecular.a;

	if (useSpecMap>0.0)
		specFactor *= specMap.Sample(linearSampler, v.Tex).r;

	float3 eyeDir = normalize(eyePos - v.posW);
	float3 ER = reflect(-eyeDir, N);
	float3 specColour = specFactor*envMap.Sample(linearSampler, ER).rgb* v.matSpecular.rgb;

	// Calculate Fresnel term
	float facing = 1-max(dot(N, eyeDir), 0);
	float fres = (FresnelBias + (1.0 - FresnelBias)*pow(abs(facing), abs(FresnelExp)));
	
	finalColour = (finalColour*(1 - fres)) + (fres * specColour);

	outputFragment.fragmentColour = float4(finalColour, baseColour.a);

	return outputFragment;

}
