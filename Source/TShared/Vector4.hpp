#pragma once
#include <initializer_list>
#include <cmath>
#include <cassert>
#include "Vector3.hpp"
#define VECTOR4F CU::Vector4<float>
namespace CU
{
	template <typename T>
	class Vector4
	{
	public:

		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector3<T>& aVector, const T& aW);
		Vector4<T>(const Vector4<T>& aVector) = default;
		Vector4<T>(const std::initializer_list<T>& aInitList);
		~Vector4<T>() = default;

		inline Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		inline Vector4<T>& operator-(void);

		inline T LengthSqr() const;
		inline T Length() const;
		inline T Distance(const Vector4<T>& aVector) const;
		inline T DistanceSqr(const Vector4<T>& aVector) const;
		inline Vector4<T> GetNormalized() const;
		inline void Normalize();
		inline void Truncate(T aUpperBound);
		inline T Dot(const Vector4<T>& aVector) const;
		Vector3<T> XYZ() const;

		T x;
		T y;
		T z;
		T w;
	};

	template <typename T>
	Vector4<T>::Vector4() :
		x(T()),
		y(T()),
		z(T()),
		w(T())
	{

	}

	template <typename T>
	Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) :
		x(aX),
		y(aY),
		z(aZ),
		w(aW)
	{

	}
	template <typename T>
	Vector4<T>::Vector4(const Vector3<T>& aVector, const T& aW) :
		x(aVector.x),
		y(aVector.y),
		z(aVector.z),
		w(aW)
	{

	}

	template <typename T>
	Vector4<T>::Vector4(const std::initializer_list<T>& aInitList) : x(), y(), z(), w()
	{
		switch (aInitList.size())
		{
			case 0:
			{
				x = T();
				y = T();
				z = T();
				w = T();
				break;
			}
			case 1:
			{
				x = *aInitList.begin();
				y = T();
				z = T();
				w = T();
				break;
			}
			case 2:
			{
				x = *aInitList.begin();
				y = *(aInitList.begin() + 1);
				z = T();
				w = T();
				break;
			}
			case 3:
			{
				x = *aInitList.begin();
				y = *(aInitList.begin() + 1);
				z = *(aInitList.begin() + 2);
				w = T();
				break;
			}
			case 4:
			{
				x = *aInitList.begin();
				y = *(aInitList.begin() + 1);
				z = *(aInitList.begin() + 2);
				w = *(aInitList.begin() + 3);
				break;
			}
			default:
			{
				assert(false && "Initializer_list wrong size");
			}
		}
	}

	template <typename T>
	inline bool operator==(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return aVector0.x == aVector1.x && aVector0.y == aVector1.y && aVector0.z == aVector1.z && aVector0.w == aVector1.w;
	}

	template <typename T>
	inline bool operator!=(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return aVector0.x != aVector1.x && aVector0.y != aVector1.y && aVector0.z != aVector1.z && aVector0.w != aVector1.w;
	}

	//Returns the vector sum of aVector0 and aVector1
	template <typename T>
	inline Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <typename T>
	inline Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
	}

	template <typename T>
	inline Vector4<T>& Vector4<T>::operator-(void)
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
		return *this;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <typename T>
	inline Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <typename T>
	inline Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <typename T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		return Vector4<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar);
	}
	template <typename T>
	Vector4<float> operator/(const Vector4<float>& aVector, const float& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		float result = 1 / aScalar;
		return Vector4<float>(aVector.x * result, aVector.y * result, aVector.z * result, aVector.w * result);
	}
	template <typename T>
	Vector4<double> operator/(const Vector4<double>& aVector, const double& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		double result = 1 / aScalar;
		return Vector4<double>(aVector.x * result, aVector.y * result, aVector.z * result, aVector.w * result);
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <typename T>
	inline void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <typename T>
	inline void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <typename T>
	inline void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <typename T>
	inline void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
		aVector.w /= aScalar;
	}
	template <typename T>
	inline void operator/=(Vector4<float>& aVector, const float& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		float result = 1 / aScalar;
		aVector.x *= result;
		aVector.y *= result;
		aVector.z *= result;
		aVector.w *= result;
	}
	template <typename T>
	inline void operator/=(Vector4<double>& aVector, const double& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		double result = 1 / aScalar;
		aVector.x *= result;
		aVector.y *= result;
		aVector.z *= result;
		aVector.w *= result;
	}

	template<typename T>
	inline T Vector4<T>::LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	}

	template<typename T>
	inline T Vector4<T>::Length() const
	{
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	template<typename T>
	inline T Vector4<T>::Distance(const Vector4<T>& aVector) const
	{
		T arg1 = x - aVector.x;
		T arg2 = y - aVector.y;
		T arg3 = z - aVector.z;
		T arg4 = w - aVector.w;
		return std::sqrt(arg1 * arg1 + arg2 * arg2 + arg3 * arg3 + arg4 * arg4);
	}

	template<typename T>
	inline T Vector4<T>::DistanceSqr(const Vector4<T>& aVector) const
	{
		T arg1 = x - aVector.x;
		T arg2 = y - aVector.y;
		T arg3 = z - aVector.z;
		T arg4 = w - aVector.w;
		return arg1 * arg1 + arg2 * arg2 + arg3 * arg3 + arg4 * arg4;
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		assert(std::sqrt(x * x + y * y + z * z + w * w) != 0 && "Division by 0");
		T result = static_cast<T>(1 / std::sqrt(x * x + y * y + z * z + w * w));
		return Vector4(x * result, y * result, z * result, w * result);
	}

	template<typename T>
	inline void Vector4<T>::Normalize()
	{
		assert(std::sqrt(x * x + y * y + z * z + w * w) != 0 && "Division by 0");
		double result = 1 / std::sqrt(x * x + y * y + z * z + w * w);
		x *= result;
		y *= result;
		z *= result;
		w *= result;
	}

	template<typename T>
	inline void Vector4<T>::Truncate(T aUpperBound)
	{
		T sqrLen = LengthSqr();
		if (sqrLen > aUpperBound * aUpperBound)
		{
			double multiplier = aUpperBound / std::sqrt(sqrLen);
			x *= multiplier; y *= multiplier;
		}
	}

	template<typename T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	}

	template <typename T>
	Vector3<T> Vector4<T>::XYZ() const
	{
		return Vector3<T>{ x, y, z };
	}
}

using Vec4f = CU::Vector4<float>;
using Vec4d = CU::Vector4<double>;
using Vec4i = CU::Vector4<int>;
using Vec4ui = CU::Vector4<unsigned int>;