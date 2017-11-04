
//
// Grass effect - Modified a fur technique
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
	float4x4			worldITMatrix;				// Correctly transform normals to world space
	float4x4			worldMatrix;
	float4				eyePos;
	float4				lightVec;					// w=1: Vec represents position, w=0: Vec  represents direction.
	float4				lightAmbient;
	float4				lightDiffuse;
	float4				lightSpecular;
	float4				windDir;
	float				Timer;
	float				grassHeight;
	float				reflectionPass;
};



//
// Textures
//

// Assumes texture bound to texture t0 and sampler bound to sampler s0
Texture2D myTexture : register(t0);
Texture2D grassAlpha: register(t1);
SamplerState anisotropicSampler : register(s0);




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

	float maxHeight = 0.005;
	float grassStage = (1.0 / maxHeight)*grassHeight;//0...1
	float tileRepeat = 32;

	FragmentOutputPacket outputFragment;

	float alpha = 1.0;
	float3 N = normalize(v.normalW);
	float3 baseColour = myTexture.Sample(anisotropicSampler, v.texCoord)*v.matDiffuse.xyz;

	outputFragment.fragmentColour = float4(baseColour, alpha);
	return outputFragment;

	////Fade from grass to dry mud and then to wet mud
	//float upperFadeStart = -0.2;
	//float lowerFadeEnd = -0.95;
	//float fadeRange = upperFadeStart - lowerFadeEnd;

	//float3  mudcolour = float3(222, 171, 106)/100.0;// / float3(159, 125, 98) / 255.0;
	//mudcolour *= (baseColour.r + baseColour.g + baseColour.b) / 3.0;// greyscale of grass to get consistent texture
	//
	//if (reflectionPass>0.0f)
	//	clip(v.posW.y < -1.0f ? -1 : 1);


	//if (v.posW.y < upperFadeStart)
	//{
	//	if (v.posW.y > lowerFadeEnd)
	//	{
	//		//fade from grass to mud
	//		float w = (v.posW.y - lowerFadeEnd) / fadeRange;
	//		baseColour = (baseColour*w) + (mudcolour*(1 - w));
	//	}
	//	else baseColour = mudcolour * 0.6;// wet mudcolour is darker;
	//}

	//// Apply lighting

	//// Apply ambient lighting
	//float3 colour = baseColour*lightAmbient.xyz;

	//// Calculate the lambertian term (essentially the brightness of the surface point based on the dot product of the normal vector with the vector pointing from v to the light source's location)
	//float3 lightDir = -lightVec.xyz; // Directional light
	//if (lightVec.w == 1.0) lightDir = lightVec.xyz - v.posW; // Positional light
	//lightDir = normalize(lightDir);
	//colour += baseColour*max(dot(lightDir, N), 0.0f)*lightDiffuse;

	//// Calculate the specular term
	//float specPower = max(v.matSpecular.a*1000.0, 1.0f);
	//float3 eyeDir = normalize(eyePos.xyz - v.posW);
	//float3 R = reflect(-lightDir, N);
	//float specFactor = pow(max(dot(R, eyeDir), 0.0f), specPower);
	//colour += baseColour*specFactor * v.matSpecular.xyz;

	//float distanceFadeMaxEnd = 10;
	//float distanceFadeMinStart = 5;
	//float distanceFadeRange = distanceFadeMaxEnd - distanceFadeMinStart;
	//float distance = length(eyePos.xyz - v.posW);

	//// only render grass that is less than distanceFadeMaxEnd
	//if (grassStage > 0.0 && distance < distanceFadeMaxEnd &&v.posW.y > lowerFadeEnd)
	//{
	//	alpha = grassAlpha.Sample(anisotropicSampler, v.texCoord*tileRepeat).a;
	//	// Alpha Test
	//	// clip(alpha < 0.9f ? -1 : 1);// clip for alpha < 0.9

	//	// Reduce alpha
	//	alpha *= (1.0 - grassStage);

	//	// and increase illumination for tips of grass
	//	colour *= (grassStage + 1.0);

	//	//fade out grass that is below upperFadeStart
	//	if (v.posW.y - lowerFadeEnd)
	//	{
	//		float w = (v.posW.y - lowerFadeEnd) / fadeRange;
	//		alpha = lerp(0, alpha, w);
	//	}

	//	//fade out grass that is further than distanceFadeMinStart and less than distanceFadeMaxEnd
	//	if (distance > distanceFadeMinStart)
	//	{
	//		float w = (distance - distanceFadeMinStart) / distanceFadeRange;
	//		alpha = lerp( alpha,0, w);
	//	}
	//}


	//outputFragment.fragmentColour = float4(colour,alpha);
	//return outputFragment;

}
