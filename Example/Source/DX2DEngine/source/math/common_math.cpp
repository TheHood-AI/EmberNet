#include "stdafx.h"
#include "math/common_math.h"
#include "math/Matrix44.h"

void Tga2D::PerspectiveFOV( float fov, float aspect, float aNear, float aFar, Matrix44& aOutMatrix )
{
	float D2R = DX2D_PI / 180.0f;
	float yScale = 1.0f / tan(D2R * fov / 2.0f);
	float xScale = yScale / aspect;
	float nearmfar = aNear - aFar;

	aOutMatrix.myXAxis4.Set(xScale, 0.0f, 0.0f, 0.0f);
	aOutMatrix.myYAxis4.Set(0.0f, yScale, 0.0f, 0.0f);
	aOutMatrix.myZAxis4.Set(0, 0, (aFar + aNear) / nearmfar, -1.0f);
	aOutMatrix.myPos4.Set(0.0f, 0.0f, 2.0f*aFar*aNear / nearmfar, 0.0f );
}

void Tga2D::MakeMatrixOrtho( float left, float right, float bottom, float top, float aNear,float aFar, Matrix44& result )
{
	result.myXAxis4.x = 2.0f / (right - left);
	result.myYAxis4.x = 0.0;
	result.myZAxis4.x = 0.0;
	result.myPos4.x = 0.0;

	//Second Column
	result.myXAxis4.y = 0.0;
	result.myYAxis4.y = 2.0f / (top - bottom);
	result.myZAxis4.y = 0.0;
	result.myPos4.y = 0.0;

	//Third Column
	result.myXAxis4.z = 0.0;
	result.myYAxis4.z = 0.0;
	result.myZAxis4.z = -2.0f / (aFar - aNear);
	result.myPos4.z = 0.0;

	//Fourth Column
	result.myXAxis4.w = -(right + left) / (right - left);
	result.myYAxis4.w = -(top + bottom) / (top - bottom);
	result.myZAxis4.w = -(aFar + aNear) / (aFar - aNear);
	result.myPos4.w = 1;
}
