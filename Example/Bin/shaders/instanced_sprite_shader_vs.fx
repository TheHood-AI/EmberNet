#include "common.fx"

struct VertexInputType
{
    float4 position : POSITION;
	uint4 myVertexIndex : TEXCOORD0;
    float2 tex : TEXCOORD1;
	float2 instancePosition : TEXCOORD2;
	float4 instanceColor : TEXCOORD3;
	float4 instanceRotationAndSize : TEXCOORD4;
	float4 instanceUV : TEXCOORD5;
	float4 instancePivot : TEXCOORD6;
	float4 myUVRect : TEXCOORD7;
};

static uint2 textureRectLookup[6] =
	{
		uint2(0, 3),
		uint2(0, 1),
		uint2(2, 3),
		uint2(2, 1),
		uint2(2, 3),
		uint2(0, 1),
	};
	
float2 GetUVRect(float4 aRect, uint aIndex)
{
	uint2 theLookuped = textureRectLookup[aIndex];
	return float2(aRect[theLookuped.x], aRect[theLookuped.y]);
}

PixelInputType VShader(VertexInputType input)
{
    PixelInputType output;
	
	float2 Resolution = myResolution.xy;
	float ratio = Resolution.y / Resolution.x;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
		
	input.position.x -= input.instancePivot.x;
	input.position.y += input.instancePivot.y;
	
	 // Update the position of the vertices based on the data for this particular instance.
    
	input.position.xy *= input.instanceRotationAndSize.yz;

	float2x2 theRotation = ComputeRotation(input.instanceRotationAndSize.x);
	input.position.xy = mul(input.position.xy, theRotation);

	input.position.x *= ratio; 	
	
	input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
	
	output.position = input.position;
   
	
	float2 textureRect = GetUVRect(input.myUVRect, input.myVertexIndex.x);
	output.tex = input.instanceUV.xy + (textureRect * input.instanceUV.zw);
	
	output.color = input.instanceColor;	
	output.textureMappingData = input.instanceRotationAndSize.w;

    return output;
}