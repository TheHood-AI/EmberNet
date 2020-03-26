#pragma once
#include "tga2d/render/render_object.h"
#include <tga2d/engine_defines.h>
namespace Tga2D
{
	class CTexture;
	class CCustomShape : public CRenderObjectCustom
	{
	public:
		CCustomShape();
		~CCustomShape();

		// Do this first, must be %3 (3, 6, 9, 12 etc..)
		int AddPoint(VECTOR2F aPoint, CColor aColor, VECTOR2F aUV = VECTOR2F(0, 0));
		void Reset();
		void RemovePoint(int aIndex);

		// Do this second
		void BuildShape();
		void Render();
		void SetTexture(const char* aPath);

		const CTexture* GetTexture() const { return myTexture; }
	private:
		CTexture* myTexture;
		bool myIsDirty;
		int myIndex;
		bool myHasTexture;
		
		
	};
}