#pragma once
#include <vector>
#include <tga2d/shaders/shader_common.h>
#include <tga2d/render/render_common.h>
#include <tga2d/arrays/tinyarrayonheap.h>

namespace Tga2D
{
	class CSprite;
	class CTexturedQuadBatch;
	class CSpriteBatch
	{
		friend class CShaderNormal;
		friend class CTexturedQuadBatchDrawer;
		friend class CShaderNormalInstanced;
		friend class CTextService;
		friend class CFontData;
	public:
		CSpriteBatch(bool aDeleteSpritesWhenDestructed);
		~CSpriteBatch();

		void Init(const char* aTexturePath = nullptr);
		bool AddObject(CSprite* aSpriteObject);
		void RemoveObject(CSprite* aSpriteObject, bool aAlsoDelete = false);

		void Render();

		void DeleteAll();

		void ClearAll();

		void SetMap(EShaderMap aMapType, const char* aPath);

		CSprite* GetNext();
		void ResetNext();

		VECTOR2UI GetImageSize();
		const std::string GetImagePath() const;
		void SetBlendState(EBlendState aBlendState);

		void SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode);


		void SetCustomShader(class CCustomShaderBatched* aShader);
	private:
		TinyGrowingArray<TinyGrowingArray<CSprite*>> mySprites;
		CTexturedQuadBatch* myQuadBatch;
		unsigned short myNextIndex;
		unsigned short myNextBatch;
		bool myDeleteSpritesOnDestruct;
	};

}

