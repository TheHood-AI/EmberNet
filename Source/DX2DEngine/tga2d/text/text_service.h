/*
This class handles all texts that will be rendered, don't use this to show text, only use the CText class
*/

#pragma once



#include <tga2d/engine_defines.h>
#include "text/fontfile.h"
#include "math/color.h"
#include "arrays/simple_pointer_array.h"
#include <unordered_map>
#include <tga2d/text/text_structures.h>

namespace Tga2D
{
	using namespace Tga2DText;
	class CFontData
	{
	public:
		CFontData()
		{
			myRefCount = 1;
			myRenderCountChanged = false;
			myLastRenderCount = 0;
			myBufferedObjects.Init(FONT_BUFF_SIZE + 1);
		}
		~CFontData();
		SCharData myCharData[1024];
		unsigned int myAtlasWidth;
		unsigned int myAtlasHeight;
		float myLineSpacing;
		unsigned int myWordSpacing;
		unsigned int myFontHeightWidth;
		int* myAtlas;
		std::string myName;
		CSpriteBatch* myBatch;
		SSimplePointerArray<STextToRender> myBufferedObjects;
		int myLastRenderCount;
		bool myRenderCountChanged;
		ID3D11ShaderResourceView* myAtlasView;
		unsigned short myRefCount;
	};


	struct TextVertexNew
	{
		TextVertexNew(){ myPosition.Set(0, 0); myTexCoord.Set(0, 0); }
		TextVertexNew(VECTOR2F aPos, VECTOR2F aTexCoord, Vector4f color = Vector4f(1, 1, 1, 1))
		{
			myPosition = aPos;
			myTexCoord = aTexCoord;
			myColor = color;
		}
		VECTOR2F myPosition;
		VECTOR2F myTexCoord;
		Vector4f myColor;
	};

	struct SText
	{
		std::string myText;
		std::string myName;
		VECTOR2F myPosition;
		float mySize;
		CColor myColor;
	};

	class CSpriteBatch;
	class CTexture;
	class CSprite;
	class CText;
	class CCustomShaderBatched;
	class CTextService
	{

	public:
		CTextService();
		~CTextService();

		void Init();
		void Update();

		CFontData* InitAndLoad(std::string aFontPathAndName, unsigned char aBorderSize, int aSize);
		void RemoveFontRef(CFontData *aData);
		bool AddTextToRender(const std::string& aText, const VECTOR2F& aPosition, CColor aColor, float aSize, CFontData* aDataPtr, float aRotation, Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith = nullptr);
		float GetSentenceWidth(Tga2D::CText* aText);
		float GetSentenceHeight(Tga2D::CText* aText);

		void ClearMemory(const std::string& aRenderName);
	private:
		bool UpdateTextAndDraw(const std::string& aText, const VECTOR2F& aPosition, CColor aColor, float aSize, CFontData* aDataPtr, float aRotation, Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith = nullptr);

		void BuildText(const std::string& aText, CColor aColor, float aSize, CFontData* aDataPtr, SSimplePointerArray<STextToRender>& aTextBuffer);
		
		int currentFontChar;
		int myCurrentSprite;
		unsigned int myTextsToDraw;

		Tga2D::CSprite* mySprites;
		struct FT_LibraryRec_* myLibrary;

		void LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY , float atlasWidth, float atlasHeight, CFontData* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth = 0);
		void LoadOutline(const int index, const int atlasX, const int atlasY , const float atlasWidth, CFontData* aFontData, struct FT_FaceRec_* aFace, int aBorderWidth);
		void CalculateOutlineOffsets(const int index, struct FT_FaceRec_* aFace, int aBorderWidth);
		void CalculateGlyphOffsets(const int index, struct FT_GlyphSlotRec_* glyph);


		void CountOffsets(const int& x, const int& y, const int& width, const int& height, SCountData& countData);
		void CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, SCountData& countData);
		std::unordered_map<std::string, CFontData*> myFontData;
		SOutlineOffset myTextOutlineOffset;
		
	};
}