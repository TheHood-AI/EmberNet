#pragma once

#include "tga2d/render/render_object.h"


namespace Tga2D
{
	class CEngine;
	class CDirectEngine;
	class CLinePrimitive : public CRenderObjectLine
	{
	public:
		CLinePrimitive(void);
		~CLinePrimitive(void);
		void Render();
		void SetFrom(float aX, float aY)
		{
			myFromPosition.Set(aX, aY);
		}
		void SetTo(float aX, float aY)
		{
			myToPosition.Set(aX, aY);
		}
	private:
	};

	class CLineMultiPrimitive : public CRenderObject
	{
	public:
		CLineMultiPrimitive(void){}
		~CLineMultiPrimitive(void){}
		void Render();
		void Draw(CDirectEngine* aRenderer) override;
		CColor *myColor;
		VECTOR2F *myFromPosition;
		VECTOR2F *myToPosition;
		unsigned int myCount;
	};
}