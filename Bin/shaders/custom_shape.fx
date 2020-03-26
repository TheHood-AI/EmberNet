#include "common.fx"

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : TEXCOORD0;
	float2 tex : TEXCOORD1;
};

struct PixelInputTypeCustom
{
    float4 position : SV_POSITION;
	float4 color : TEXCOORD0;
	float textureMappingData : TEXCOORD1;
	float2 tex : TEXCOORD2;
};

Texture2D shaderTexture : register( t0 );
PixelInputTypeCustom VShader(VertexInputType input)
{
    PixelInputTypeCustom output;
	

    // Change the position vector to be 4 units for proper matrix calculations.
	
    input.position.w = 1.0f;	
	
			
	float2x2 theRotation = ComputeRotation(myRotationAndSizeCustom.x);
	input.position.xy = mul(input.position.xy, theRotation);
	
	
	input.position.x *= myRotationAndSizeCustom.y;
	input.position.y *= myRotationAndSizeCustom.z;
	
			
    // Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = input.position;
	output.position.xy +=myPositionCustom.xy;
	output.tex = input.tex;
    
	output.color = input.color;
	output.textureMappingData = myRotationAndSizeCustom.w;
	
    return output;
}

#ifdef USE_LIGHTS
float4 CalculateLight(PixelInputTypeCustom input)
{
	float4 Diffuse = input.color;
	float3 AmbientColor = Diffuse.xyz * myAmbience;
	float3 DirectColor = float3(0, 0, 0);

	float2 Resolution = myResolution.xy;
	for (unsigned int index = 0; index < myNumberOfLights; index++)
	{
		float Intensity = myLights[index].myPosition.z;
		float Falloff = myLights[index].myPosition.w;

		float2 difference = float2((myLights[index].myPosition.xy - input.position.xy) / Resolution.xy);
		difference.x *= (Resolution.x / Resolution.y);
		difference.y *= -1; 

		float distance = length(difference); 

		float linearAttenuation = saturate((Falloff - distance) / Falloff);
		float physicalAttenuation = 1.0f/(1.0f + distance);
		float totalAttenuation = linearAttenuation * physicalAttenuation;

		float3 direction = normalize(float3(difference.xy, (Falloff / 2)));
		DirectColor += Diffuse.xyz * Intensity * totalAttenuation * myLights[index].myLightColor.xyz;		
	} 	

	return float4(AmbientColor + DirectColor, Diffuse.w);
}
#endif

SamplerState SampleType;
float4 PShader(PixelInputTypeCustom  input) : SV_TARGET
{
	float4 Diffuse = shaderTexture.SampleLevel(SampleType, input.tex, 0) * input.color;
	input.color = Diffuse;
	#ifdef USE_LIGHTS
	if(input.textureMappingData > 0.0f)
	{
		return CalculateLight(input);
	}
	#endif
    return input.color;
}