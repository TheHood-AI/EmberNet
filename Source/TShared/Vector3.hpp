#pragma once
#include <initializer_list>
#include <cmath>
#include <cassert>
#define VECTOR3F CU::Vector3<float>
namespace CU
{
	template <typename T>
	class Vector3
	{
	public:

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>(const std::initializer_list<T>& aInitList);
		~Vector3<T>() = default;

		inline Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		inline Vector3<T>& operator-(void);

		inline T LengthSqr() const;
		inline T Length() const;
		inline T Distance(const Vector3<T>& aVector) const;
		inline T DistanceSqr(const Vector3<T>& aVector) const;
		inline Vector3<T> GetNormalized() const;
		inline void Normalize();
		inline void Truncate(T aUpperBound);
		inline T Dot(const Vector3<T>& aVector) const;
		inline Vector3<T> Cross(const Vector3<T>& aVector) const;

		T x;
		T y;
		T z;
	};

	template <typename T>
	Vector3<T>::Vector3() :
		x (T()),
		y (T()),
		z (T())
	{

	}

	template <typename T>
	Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) :
  		x (aX),
		y (aY),
		z (aZ)
	{

	}

	template <typename T>
	Vector3<T>::Vector3(const std::initializer_list<T>& aInitList): x(0), y(0), z(0)
	{
		switch (aInitList.size())
		{
			case 0:
			{
				x = T();
				y = T();
				z = T();
				break;
			}
			case 1:
			{
				x = *aInitList.begin();
				y = T();
				z = T();
				break;
			}
			case 2:
			{
				x = *aInitList.begin();
				y = *(aInitList.begin() + 1);
				z = T();
				break;
			}
			case 3:
			{
				x = *aInitList.begin();
				y = *(aInitList.begin() + 1);
				z = *(aInitList.begin() + 2);
				break;
			}
			default:
			{
				assert(false && "Initializer_list wrong size");
			}
		}
	}

	template <typename T>
	inline bool operator==(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return aVector0.x == aVector1.x && aVector0.y == aVector1.y && aVector0.z == aVector1.z;
	}

	template <typename T>
	inline bool operator!=(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return aVector0.x != aVector1.x && aVector0.y != aVector1.y && aVector0.z != aVector1.z;
	}

	//Returns the vector sum of aVector0 and aVector1
	template <typename T>
	inline Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <typename T>
	inline Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}

	template <typename T>
	inline Vector3<T>& Vector3<T>::operator-(void)
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <typename T> 
	inline Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <typename T> 
	inline Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector) 
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <typename T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) 
	{
		assert(aScalar != 0 && "Division by 0");
		return Vector3<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar);
	}
	template <typename T>
	Vector3<float> operator/(const Vector3<float>& aVector, const float& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		float result = 1 / aScalar;
		return Vector3<float>(aVector.x * result, aVector.y * result, aVector.z * result);
	}
	template <typename T>
	Vector3<double> operator/(const Vector3<double>& aVector, const double& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		double result = 1 / aScalar;
		return Vector3<double>(aVector.x * result, aVector.y * result, aVector.z * result);
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <typename T> 
	inline void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <typename T> 
	inline void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <typename T> 
	inline void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <typename T> 
	inline void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}
	template <typename T>
	inline void operator/=(Vector3<float>& aVector, const float& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		float result = 1 / aScalar;
		aVector.x *= result;
		aVector.y *= result;
		aVector.z *= result;
	}
	template <typename T>
	inline void operator/=(Vector3<double>& aVector, const double& aScalar)
	{
		assert(aScalar != 0 && "Division by 0");
		double result = 1 / aScalar;
		aVector.x *= result;
		aVector.y *= result;
		aVector.z *= result;
	}

	template<typename T>
	inline T Vector3<T>::LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	template<typename T>
	inline T Vector3<T>::Length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	template<typename T>
	inline T Vector3<T>::Distance(const Vector3<T>& aVector) const
	{
		T arg1 = x - aVector.x;
		T arg2 = y - aVector.y;
		T arg3 = z - aVector.z;
		return std::sqrt(arg1 * arg1 + arg2 * arg2 + arg3 * arg3);
	}

	template<typename T>
	inline T Vector3<T>::DistanceSqr(const Vector3<T>& aVector) const
	{
		T arg1 = x - aVector.x;
		T arg2 = y - aVector.y;
		T arg3 = z - aVector.z;
		return arg1 * arg1 + arg2 * arg2 + arg3 * arg3;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		if(std::sqrt(x * x + y * y + z * z) == 0)
		{
			return Vector3(x, y, z);
		}
		//assert(std::sqrt(x * x + y * y + z * z) != 0 && "Division by 0");
		T result = 1 / std::sqrt(x * x + y * y + z * z);
		return Vector3(x * result, y * result, z * result);
	}

	template<typename T>
	inline void Vector3<T>::Normalize()
	{
		const double sqr = std::sqrt(x * x + y * y + z * z);
		if(sqr == 0)
		{
			return;
		}
		double result = 1 / sqr;
		x *= result;
		y *= result;
		z *= result;
	}

	template<typename T>
	inline void Vector3<T>::Truncate(T aUpperBound)
	{
		T sqrLen = LengthSqr();
		if (sqrLen > aUpperBound * aUpperBound)
		{
			double multiplier = aUpperBound / std::sqrt(sqrLen);
			x *= multiplier; y *= multiplier;
		}
	}

	template<typename T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		return Vector3(y * aVector.z - z * aVector.y, z * aVector.x - x * aVector.z, x * aVector.y - y * aVector.x);
	}
}

//using Vec3f = CU::Vector2<float>;
//using Vec3d = CU::Vector2<double>;
//using Vec3i = CU::Vector2<int>;
//using Vec3ui = CU::Vector2<unsigned int>;