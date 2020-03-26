#pragma once
#include "Vector3.hpp"
namespace CommonUtilities
{
	template <class T>
	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(const Sphere<T>& aSphere);
		Sphere(const CU::Vector3<T>& aCenter, T aRadius);
		void InitWithCenterAndRadius(const CU::Vector3<T>& aCenter, T aRadius);
		bool IsInside(const CU::Vector3<T>& aPosition) const;
		CU::Vector3<T> GetCenter()const;
		T GetRadius()const;
	private:
		T myRadius;
		CU::Vector3<T> myCenter;
	};

	template<class T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myRadius = aSphere.myRadius;
		myCenter = aSphere.myCenter;
	}

	template<class T>
	inline Sphere<T>::Sphere(const CU::Vector3<T>& aCenter, T aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	}

	template<class T>
	inline void Sphere<T>::InitWithCenterAndRadius(const CU::Vector3<T>& aCenter, T aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	}

	template<class T>
	inline bool Sphere<T>::IsInside(const CU::Vector3<T>& aPosition) const
	{
		return (aPosition - myCenter).LengthSqr() <= pow(myRadius, 2);
	}
	template<class T>
	inline CU::Vector3<T> Sphere<T>::GetCenter() const
	{
		return myCenter;
	}
	template<class T>
	inline T Sphere<T>::GetRadius() const
	{
		return myRadius;
	}
}