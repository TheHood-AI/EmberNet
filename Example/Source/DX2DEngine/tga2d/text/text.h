/*
Use this class to create and show a text
*/

#pragma once
#include <tga2d/math/Color.h>
#include <tga2d/math/vector2.h>
#include <tga2d/render/render_common.h>
#include <string>

namespace Tga2D
{
	enum EFontSize
	{
		EFontSize_6 = 6,
		EFontSize_8 = 8,
		EFontSize_9 = 9,
		EFontSize_10 = 10,
		EFontSize_11 = 11,
		EFontSize_12 = 12,
		EFontSize_14 = 14,
		EFontSize_18 = 18,
		EFontSize_24 = 24,
		EFontSize_30 = 30,
		EFontSize_36 = 36,
		EFontSize_48 = 48,
		EFontSize_60 = 60,
		EFontSize_72 = 72,
		EFontSize_Count
	};
	class CTextService;
	class CCustomShaderBatched;
	class CText
	{
		friend class CTextService;
	public:
		// If this is the first time creating the text, the text will be loaded in memory, dont do this runtime

		/*aPathAndName: ex. taxe/arial.ttf, */
		CText(const char* aPathAndName = "Text/arial.ttf", EFontSize aFontSize = EFontSize_14, unsigned char aBorderSize = 0);
		~CText();
		void Render();
		void Render(Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith);
		float GetWidth();
		float GetHeight();

		void SetColor(const CColor& aColor);
		CColor GetColor() const;

		void SetText(const std::string& aText);
		std::string GetText() const;

		void SetPosition(const VECTOR2F& aPosition);
		VECTOR2F GetPosition() const;

		void SetScale(float aScale);
		float GetScale() const;

		std::string GetPathAndName() const;

		void SetRotation(float aRotation) { myRotation = aRotation; }
		float GetRotation() const { return myRotation; }

		
	protected:
		CTextService* myTextService;
		class CFontData* myFontDataPointer;
		std::string myPathAndName;
		std::string myText;
		VECTOR2F myPosition;
		float myScale;
		float myRotation;
		CColor myColor;
		
		
	};
}

