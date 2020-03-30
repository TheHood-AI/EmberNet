#pragma once
#include "Vector3.hpp"
namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		AABB3D() = default;
		AABB3D(const AABB3D<T>& aAABB3D);
		AABB3D(const CU::Vector3<T>& aMin, const CU::Vector3<T>& aMax);
		void InitWithMinAndMax(const CU::Vector3<T>& aMin, const CU::Vector3<T>& aMax);
		bool IsInside(const CU::Vector3<T>& aPosition) const;
		CU::Vector3<T> GetMin() const;
		CU::Vector3<T> GetMax() const;
	private:
		CU::Vector3<T> myMin;
		CU::Vector3<T> myMax;


	};

	template<class T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMin = aAABB3D.myMin;
		myMax = aAABB3D.myMax;
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const CU::Vector3<T>& aMin, const CU::Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<class T>
	inline void AABB3D<T>::InitWithMinAndMax(const CU::Vector3<T> & aMin, const CU::Vector3<T> & aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<class T>
	inline bool AABB3D<T>::IsInside(const CU::Vector3<T>& aPosition) const
	{
		if (aPosition.x >= myMin.x && aPosition.x <= myMax.x)
		{
			if (aPosition.y >= myMin.y && aPosition.y <= myMax.y)
			{
				if (aPosition.z >= myMin.z && aPosition.z <= myMax.z)
				{
					return true;
				}
			}
		}
		return false;
	}

	template<class T>
	inline CU::Vector3<T> AABB3D<T>::GetMin() const
	{
		return myMin;
	}

	template<class T>
	inline CU::Vector3<T> AABB3D<T>::GetMax() const
	{
		return myMax;
	}

}