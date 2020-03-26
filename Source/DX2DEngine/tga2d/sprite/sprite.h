/*
Use this class to create and show a sprite
*/

#pragma once
#include "tga2d/render/render_common.h"
#include "tga2d/math/Color.h"
namespace Tga2D
{
	
	class CTexturedQuad;
	class CTexturedQuadBatch;
	struct STextureRext;
	class CSprite
	{
		friend class CTexturedQuadBatchDrawer;
		friend class CShaderNormalInstanced;
		friend class CShaderDistanceFieldInstanced;
		friend class CSpriteBatch;
		friend class CVideo;
		friend class CDirectEngine;
		friend class CCustomShader;
		friend class CTextService;
	public:
		CSprite();
		/* If you want the texture size and properties, send in the path inspite of it being a part of a batch*/
		CSprite(const char* aTexturePath);
		CSprite(const CSprite& aCSprite);
		~CSprite();

		CSprite& operator=(const CSprite& aCSprite);
		
		void Init(const char* aTexturePath);
		void Render();
		/* Set a normalized position on the sprite*/
		void SetPosition(const VECTOR2F& aPosition);
		const VECTOR2F& GetPosition() const;

		void SetPivot(const VECTOR2F& aPivot);
		const VECTOR2F& GetPivot() const;

		/* Set a rotation in radians*/
		void SetRotation(const float aRotationInRadians);
		float GetRotation() const;

		void SetColor(const CColor& aColor);
		const CColor& GetColor() const;

		void SetSizeRelativeToScreen(const VECTOR2F& aSize);
		void SetSizeRelativeToImage(const VECTOR2F& aSize);

		virtual VECTOR2F GetSize() const;

		/* Image size in pixels */
		virtual VECTOR2UI GetImageSize() const;

		void SetUVScale(VECTOR2F aScale);
		const VECTOR2F& GetUVScale() const;

		bool HasValidTexture() const;

		/*
		aX = The texture region top-left corner X-coordinate.
		aY = The texture region top-left corner Y-coordinate.
		aEndX =  The texture region top-right corner X-coordinate.
		aEndY = The texture region bottom-right corner X-coordinate.
		*/
		void SetTextureRect(float aStartX, float aStartY, float aEndX, float aEndY);
		STextureRext* GetTextureRect();

		bool HasSameTextureAs(CSprite* aSprite) const;

		void SetUVOffset(const VECTOR2F& aUV);
		const VECTOR2F& GetUVOffset() const;
	

		/* If this sprite is rendered by a spritebatch, this is useful to not render the sprite but keep it in the batch*/
		void SetShouldRender(bool aShouldRender){ myShouldRender = aShouldRender; }
		bool GetShouldRender() const { return myShouldRender; }

		void SetMap(const EShaderMap aMapType, const char* aPath);

		/* Will delete the texturedata, be aware that if anything else uses this texture it will be invalid*/
		void ReleaseAttachedTexture();

		/* Initially expensive! Once this has been called once, the data is stored and will be cheap. But run this once in your INIT function, then it will be fine*/
		CColor GetPixelColor(unsigned short aX, unsigned short aY);

		const char* GetImagePath() const;

		void SetCustomShader(class CCustomShader* aShader);

		void SetBlendState(EBlendState aBlendState);
		void SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode);
		
		
	private:
		void ClearBatchData();
		bool HasChangedSize() const { return myHasChangedSize; }
		void InternalInit(const char* aTexturePath);
		CTexturedQuad *myQuad;
		bool myHasChangedSize;
		bool myShouldRender;
		bool myIsPartOfbatch;
		bool myShouldRenderBatched;
	protected:
		CTexturedQuad* GetTexturedQuad() const { return myQuad; }
		
	};
}