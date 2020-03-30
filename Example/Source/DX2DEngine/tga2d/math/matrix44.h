#pragma once
#include "Vector4.h"
#include "common_math.h"

namespace Tga2D
{
	class Matrix44
	{
	public:
		__forceinline Matrix44()
		{
			myXAxis4.x = 1.0f;
			myXAxis4.y = 0.0f;
			myXAxis4.z = 0.0f;
			myXAxis4.w = 0.0f;

			myYAxis4.x = 0.0f;
			myYAxis4.y = 1.0f;
			myYAxis4.z = 0.0f;
			myYAxis4.w = 0.0f;

			myZAxis4.x = 0.0f;
			myZAxis4.y = 0.0f;
			myZAxis4.z = 1.0f;
			myZAxis4.w = 0.0f;

			myPos4.x = 0.0f;
			myPos4.y = 0.0f;
			myPos4.z = 0.0f;
			myPos4.w = 1.0f;
		}
		__forceinline Matrix44& Transpose(); // Transpose as function affects and returns this

		Vector4f myXAxis4;
		Vector4f myYAxis4;
		Vector4f myZAxis4;
		Vector4f myPos4;
	};



	Matrix44& Matrix44::Transpose()
	{
		float fTemp;
		fTemp = myXAxis4.y;
		myXAxis4.y = myYAxis4.x;
		myYAxis4.x = fTemp;

		fTemp = myXAxis4.z;
		myXAxis4.z = myZAxis4.x;
		myZAxis4.x = fTemp;

		fTemp = myXAxis4.w;
		myXAxis4.w = myPos4.x;
		myPos4.x = fTemp;

		fTemp = myYAxis4.z;
		myYAxis4.z = myZAxis4.y;
		myZAxis4.y = fTemp;

		fTemp = myYAxis4.w;
		myYAxis4.w = myPos4.y;
		myPos4.y = fTemp;

		fTemp = myZAxis4.w;
		myZAxis4.w = myPos4.z;
		myPos4.z = fTemp;

		return *this;
	}



}
