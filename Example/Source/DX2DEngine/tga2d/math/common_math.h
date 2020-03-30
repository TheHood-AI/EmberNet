#pragma once
#include <math.h>


#define DX2D_PI 3.14159265358979323846f
#define Ftrunc(aFloat) ((float)((int)(aFloat)))
#define FmodMacro(aX, aY) ((aX) - Ftrunc((aX) * (1.0f / (aY))) * (aY))


namespace Tga2D
{
	class Matrix44;
	void PerspectiveFOV(float fov, float aspect, float aNear, float aFar, Matrix44& aOutMatrix);
	void MakeMatrixOrtho(float left, float right, float bottom, float top, float aNear,float aFar, Matrix44& result);

	__forceinline float DX2D_Cos(const float aX)
	{
		// Calculate abs value (cos(x) == cos(-x))
		float t = fabsf(aX);

		// Mod to [0 .. 2PI] range
		t = FmodMacro(t, 2.0f * DX2D_PI);

		// Reflect in PI
		t = fabsf(t - DX2D_PI);

		// Offset to sin space
		t -= 0.5f * DX2D_PI;

		// Taylor series constants
		const float c0 = -1.0f / 6.0f;			// 1/3!
		const float c1 = 1.0f / 120.f;			// 1/5! 
		const float c2 = -1.0f / 5040.f;		// 1/7! 
		const float c3 = 1.0f / 362880.f;		// 1/9! 
		const float c4 = -1.0f / 39916800.f;	// 1/11! 

		// Do Taylor approximation
		const float t2 = t * t;
		return t + t * (t2 * (c0 + t2 * (c1 + t2 * (c2 + t2 * (c3 + t2 * c4)))));
	}

	__forceinline float DX2D_Sin(float aX)
	{
		return DX2D_Cos(aX - 0.5f * DX2D_PI);
	}

	__forceinline void DX2D_SinCos(float aX, float* aOutSin, float* aOutCos)
	{
		*aOutSin = DX2D_Sin(aX);
		*aOutCos = DX2D_Cos(aX);
	}

	const double Pi = 3.14159265358979323846264338327950288419716939937510; 
	const float Pif = 3.14159265358979323846264338327950288419716939937510f;

	inline double RadToDeg(double val) { return (val * 180.0/Pi); }
	inline float RadToDeg(float val) { return (val * 180.0f/Pif); }

	inline double DegToRad(double val) { return (val * Pi/180.0); }
	inline float DegToRad(float val) { return (val * (float)Pi/180.0f); }

	template <class T>
	inline T Sign(const T& val) { return (val >= T(0)) ? T(1) : T(-1); }

	template <class T, class T2, class T3>
	inline T Clamp(T val, T2 mn, T3 mx) { return (T) ((val > (T) mx) ? (T) mx : (val < (T) mn) ? (T) mn : val); }

	template <class T, class T2, class T3>
	inline T Wrap(T val, T2 mn, T3 mx) { T d = (T)(mx - mn); while (val > (T)mx) val -= d; while (val < (T)mn) val += d; return val; }



}
