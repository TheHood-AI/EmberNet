#pragma once
#include <vector>
#include "tga2d/math/Color.h"
#include "render_common.h"
namespace Tga2D
{
	class CDirectEngine;
	class CRenderObject
	{
		friend class CDirectEngine;
	public:
		CRenderObject();
		virtual ~CRenderObject(){}

		void SetBlendState(EBlendState aBlendState) { myBlendState = aBlendState; }
		void SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode) { mySamplerFilter = aFilter; mySamplerAddressMode = aAddressMode; }
		bool myIsScreenSpace;
		EBlendState myBlendState;
		ESamplerFilter mySamplerFilter;
		ESamplerAddressMode mySamplerAddressMode;

	protected:
		virtual void Draw(CDirectEngine* aRenderer) = 0;
	};

	class CRenderObjectSprite : public CRenderObject
	{
	public:
		CRenderObjectSprite();
		~CRenderObjectSprite(){}
		void Draw(CDirectEngine* aRenderer) override;
		VECTOR2F myPosition;
		VECTOR2F myPivot;		
		VECTOR2F mySize;
		VECTOR2F mySizeMultiplier;
		VECTOR2UI myImageSize;
		VECTOR2F myUV;
		VECTOR2F myUVScale;
		CColor myColor;
		float myRotation;
	};

	class CRenderObjectLine : public CRenderObject
	{
	public:
		CRenderObjectLine();
		virtual ~CRenderObjectLine(){}
		void Draw(CDirectEngine* aRenderer) override;

		Vector4f myColor;
		VECTOR2F myFromPosition;
		VECTOR2F myToPosition;
	};


	struct SCustomPoint
	{
		SCustomPoint()
		{
			myPosition.Set(0, 0);
			myColor.Set(1, 1, 1, 1);
			myUV.Set(0.5f, 0.5f);
			myIndex = 0;
		}
		VECTOR2F myPosition;
		CColor myColor;
		VECTOR2F myUV;
		int myIndex;
	};
	class CRenderObjectCustom : public CRenderObject
	{
	public:
		CRenderObjectCustom();
		~CRenderObjectCustom(){}
		void Draw(CDirectEngine* aRenderer) override;
		std::vector<SCustomPoint> myPoints;

		void SetSize(const VECTOR2F& aSize)
		{
			mySize = aSize;
		}
		const VECTOR2F& GetSize()
		{
			return mySize;
		}

		void SetPosition(const VECTOR2F& aPosition)
		{
			myPosition = aPosition;
		}
		const VECTOR2F& GetPosition()
		{
			return myPosition;
		}

		void SetRotation(const float aRotation)
		{
			myRotation = aRotation;
		}
		const float GetRotation()
		{
			return myRotation;
		}

		EBlendState myBlendState;

	private:
		VECTOR2F mySize;
		VECTOR2F myPosition;
		float myRotation = 0;
	};
}