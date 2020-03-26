#pragma once
#include "Vector4.h"

namespace Tga2D
{
	class CColor
	{
	public:
		CColor()
			:myR(0.0f)
			, myG(0.0f)
			, myB(0.0f)
			, myA(0.0f)

		{}

		CColor(float aR, float aG, float aB, float aA)
		{
			myR = aR;
			myG = aG;
			myB = aB;
			myA = aA;
		}

		void Set(float aR, float aG, float aB, float aA)
		{
			myR = aR;
			myG = aG;
			myB = aB;
			myA = aA;
		}

		Vector4f AsVec4()
		{
			return Vector4f(myR, myG, myB, myA);
		}

		unsigned int AsHex()
		{
			unsigned char r = static_cast<unsigned char>(myR * 255.0f);
			unsigned char g = static_cast<unsigned char>(myG * 255.0f);
			unsigned char b = static_cast<unsigned char>(myB * 255.0f);
			unsigned char a = static_cast<unsigned char>(myA * 255.0f);

			unsigned int final = 0;
			final |= (a << 24);
			final |= (r << 16);
			final |= (g << 8);
			final |= (b);
			return final;
		}

		float myR;
		float myG;
		float myB;
		float myA;
	};
}