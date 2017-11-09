
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


	float3 N = normalize(v.normalW);

	float4 baseColour = v.matDiffuse;

	baseColour = baseColour * myTexture.Sample(linearSampler, v.texCoord);

	//Initialise returned colour to ambient component
	float3 colour = baseColour.xyz * (lightAmbient.xyz + light2Ambient.xyz);

	// Calculate the lambertian term (essentially the brightness of the surface point based on the dot product of the normal vector with the vector pointing from v to the light source's location)
	float3 lightDir = -lightVec.xyz; // Directional light
	if (lightVec.w == 1.0)
		lightDir =lightVec.xyz - v.posW; // Positional light
	lightDir=normalize(lightDir);

	float3 light2Dir = -light2Vec.xyz; // Directional light
		if (light2Vec.w == 1.0)
			light2Dir = light2Vec.xyz - v.posW; // Positional light
	light2Dir = normalize(light2Dir);

	// Add diffuse lights if relevant (otherwise we end up just returning the ambient light colour)
	colour += max(dot(lightDir, N), 0.0f) *  baseColour.xyz * lightDiffuse;
	colour += max(dot(light2Dir, N), 0.0f) * baseColour.xyz * light2Diffuse;

	// Calc specular light
	float specPower = max(v.matSpecular.a*1000.0, 1.0f);
	float3 eyeDir = normalize(eyePos - v.posW);
	
	float3 R = reflect(-lightDir, N);
	float3 R2 = reflect(-light2Dir, N);

	float specFactor = pow(max(dot(R, eyeDir), 0.0f), specPower);
	float specFactor2 = pow(max(dot(R2, eyeDir), 0.0f), specPower);

	colour += specFactor  * v.matSpecular.xyz * lightSpecular;
	colour += specFactor2 * v.matSpecular.xyz * light2Specular;

	outputFragment.fragmentColour = float4(colour, baseColour.a);
	return outputFragment;

}
