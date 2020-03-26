#include "stdafx.h"

#include "text/text.h"
#include "engine.h"
#include "text/text_service.h"
#include <tga2d/shaders/customshader.h>
using namespace Tga2D;

CText::CText(const char* aPathAndName, EFontSize aFontSize, unsigned char aBorderSize)
: myTextService(&Tga2D::CEngine::GetInstance()->GetTextService())
{
	myColor.Set(1, 1, 1, 1);
	myScale = 1.0f;
	myPathAndName = aPathAndName;
	myFontDataPointer = myTextService->InitAndLoad(myPathAndName, aBorderSize, aFontSize);
	myRotation = 0.0f;
}


CText::~CText()
{
	if (Tga2D::CEngine* engine = Tga2D::CEngine::GetInstance())
	{
		engine->GetTextService().RemoveFontRef(myFontDataPointer);
	}
}

void Tga2D::CText::Render()
{
	if (!myTextService)
	{
		return;
	}
	if (!myTextService->AddTextToRender(myText, myPosition, myColor, myScale, myFontDataPointer, myRotation))
	{
		ERROR_PRINT("%s", "Text rendering error! Trying to render a text where the resource has been deleted! Did you clear the memory for this font? OR: Did you set the correct working directory?");
	}
}

void Tga2D::CText::Render(Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith)
{
	if (!myTextService)
	{
		return;
	}
	if (!myTextService->AddTextToRender(myText, myPosition, myColor, myScale, myFontDataPointer, myRotation, aCustomShaderToRenderWith))
	{
		ERROR_PRINT("%s", "Text rendering error! Trying to render a text where the resource has been deleted! Did you clear the memory for this font? OR: Did you set the correct working directory?");
	}
}

float Tga2D::CText::GetWidth()
{
	if (!myTextService)
	{
		return 0.0f;
	}

	return myTextService->GetSentenceWidth(this) / CEngine::GetInstance()->GetWindowSize().x;
}

float Tga2D::CText::GetHeight()
{
	if (!myTextService)
	{
		return 0.0f;
	}

	return myTextService->GetSentenceHeight(this) / CEngine::GetInstance()->GetWindowSize().y;
}

void Tga2D::CText::SetColor(const CColor& aColor)
{
	myColor = aColor;
}

Tga2D::CColor Tga2D::CText::GetColor() const
{
	return myColor;
}

void Tga2D::CText::SetText(const std::string& aText)
{
	myText = aText;
}

std::string Tga2D::CText::GetText() const
{
	return myText;
}

void Tga2D::CText::SetPosition(const VECTOR2F& aPosition)
{
	myPosition = aPosition;
}

VECTOR2F Tga2D::CText::GetPosition() const
{
	return myPosition;
}

void Tga2D::CText::SetScale(float aScale)
{
	myScale = aScale;
}

float Tga2D::CText::GetScale() const
{
	return myScale;
}

std::string Tga2D::CText::GetPathAndName() const
{
	return myPathAndName;
}
