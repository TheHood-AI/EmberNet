#include "pch.h"
#include "Random.hpp"
#include <random>

namespace CU
{
	int RandomInt(const int aMin, const int aMax)
	{
		static std::random_device rndDevice;
		static std::mt19937 mt(rndDevice());

		const std::uniform_int_distribution <int> rndDist(aMin, aMax);

		return rndDist(mt);
	}

	float RandomFloat(const float aMin, const float aMax)
	{
		static std::random_device rndDevice;
		static std::mt19937 mt(rndDevice());

		const std::uniform_real_distribution<float> rndDist(aMin, aMax);

		return rndDist(mt);
	}

	int RandomOfTwoNumbers(const int aFirst, const int aSecond)
	{
		static std::random_device rndDevice;
		static std::mt19937 mt(rndDevice());

		const std::uniform_int_distribution<int> rndDist(1, 2);

		const int nr = rndDist(mt);
		if (nr == 1)
		{
			return aFirst;
		}

		return aSecond;
	}
}