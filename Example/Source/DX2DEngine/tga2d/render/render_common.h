#pragma once
#include "tga2d/math/Vector2.h"

namespace Tga2D
{

	enum EShader
	{
		EShader_Normal,
		EShader_FullScreen,
		EShader_MAX
	};

	enum EBlendState
	{
		EBlendState_Disabled,
		EBlendState_Alphablend,
		EBlendState_Additiveblend,
		EBlendState_Count
	};
	enum ESamplerFilter
	{
		ESamplerFilter_Point,
		ESamplerFilter_Bilinear,
		ESamplerFilter_Trilinear,
		ESamplerFilter_Count
	};

	enum ESamplerAddressMode
	{
		ESamplerAddressMode_Clamp,
		ESamplerAddressMode_Wrap,
		ESamplerAddressMode_Mirror,
		ESamplerAddressMode_Count
	};

	enum EShaderMap
	{
		NORMAL_MAP,
		MAP_MAX,
	};

#pragma warning( push )
#pragma warning( disable : 26495)

	struct SColor
	{
		SColor() {}
		SColor(float aR, float aG, float aB, float aA)
			:myR(aR)
			, myG(aG)
			, myB(aB)
			, myA(aA)
		{}
		float myR, myG, myB, myA;    // color
	};

#pragma warning( pop )

	struct SVertex
	{
		float X, Y, Z;      // position
		float texX, texY;    // tex
	};

	struct SVertexInstanced
	{
		float X, Y, Z, W;      // position
		unsigned int myVertexIndex, unused1, unused2, unused3;
		float texX, texY;    // tex
	};

	struct SSimpleVertex
	{
		float X, Y, Z;      // position
		float myColorR, myColorG, myColorB, myColorA;
		float myU, myV;
	};
}