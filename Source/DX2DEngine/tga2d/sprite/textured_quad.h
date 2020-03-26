#pragma once
#include "render/render_object.h"
#include "render/render_common.h"

namespace Tga2D
{
	struct STextureRext
	{
		float myStartX;
		float myStartY;
		float myEndX;
		float myEndY;
	};
	struct ID3D11Resource;
	class CEngine;
	class CTextureManager;
	class CTexture;
	class CTexturedQuad : public CRenderObjectSprite
	{
		friend class CShaderNormalInstanced;
		friend class CSprite;
	public:
		CTexturedQuad(void);
		~CTexturedQuad(void);
		void Init(const char* aTexturePath);
		void Render();
		void SetTextureRect(float aStartX, float aStartY, float aEndX, float aEndY);
		CEngine* myEngine;	
		CTexture* myTexture;

		STextureRext& GetTextureRect(){return myTextureRect;}
		void SetColor(const CColor aColor);
		const CColor& GetColor() const;
		void SetSizeRelativeToScreen(const VECTOR2F& aSize);
		void SetSizeRelativeToImage(const VECTOR2F& aSize);
		virtual void SetUV(const VECTOR2F& aUV);
		void SetMap(EShaderMap aMapType, const char* aPath);


		void SetShader(EShader aShaderType) { myShaderType = aShaderType; }
		EShader GetShaderType() const { return myShaderType; }
		
		CTexture* myMaps[MAP_MAX];
		class CCustomShader* myCustomShader;
	protected:
		STextureRext myTextureRect;
		EShader myShaderType;
	};
}
