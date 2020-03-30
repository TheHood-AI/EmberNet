#pragma once
#include <tga2d/math/vector2.h>
#include <tga2d/math/color.h>
#define SPRITE_BATCH_COUNT 1024
namespace Tga2D
{
	enum EConstantBufferSlot
	{
		EConstantBufferSlot_Matrix = 0,
		EConstantBufferSlot_Common = 1,
		EConstantBufferSlot_Light = 2,
		EConstantBufferSlot_Object = 3,
		EConstantBufferSlot_ObjectCustom = 4,
		EConstantBufferSlot_MAX,
	};
	enum SEffect
	{
		SEffect_None,
	};
	struct ObjectBufferInstanceType
	{
		ObjectBufferInstanceType()
		{
			myPosition.Set(0, 0);
		}
		VECTOR2F myPosition;
		Vector4f myColor;
		Vector4f myRotationAndSize;
		Vector4f myUV;
		Vector4f myPivot;
		Vector4f myUVRect;
	};

}