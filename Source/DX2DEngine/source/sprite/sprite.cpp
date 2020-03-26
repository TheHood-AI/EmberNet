#include "stdafx.h"

#include "sprite/sprite.h"
#include "sprite/textured_quad.h"
#include "texture/texture_manager.h"
#include "shaders/customshader.h"

using namespace Tga2D;
Tga2D::CSprite::CSprite(const char* aTexturePath)
	:myQuad(nullptr)
	,myHasChangedSize(false)
	,myShouldRender(true)
	,myIsPartOfbatch(false)
	, myShouldRenderBatched(false)
{
	InternalInit(aTexturePath);
}



Tga2D::CSprite::CSprite()
	:myQuad(nullptr)
	, myHasChangedSize(false)
	, myShouldRender(true)
	, myIsPartOfbatch(false)
	, myShouldRenderBatched(false)
{
	InternalInit(nullptr);
}


Tga2D::CSprite::~CSprite(void)
{
	delete myQuad;
	myQuad = nullptr;
}

CSprite::CSprite(const CSprite& aCSprite)
{
	myQuad = nullptr;
	*this = aCSprite;
}

CSprite& Tga2D::CSprite::operator=(const CSprite& aCSprite)
{
	myHasChangedSize = aCSprite.myHasChangedSize;
	myShouldRender = aCSprite.myShouldRender;
	myIsPartOfbatch = aCSprite.myIsPartOfbatch;
	myShouldRenderBatched = aCSprite.myShouldRenderBatched;
	if (aCSprite.myQuad != nullptr)
	{
		if (myQuad == nullptr)
		{
			myQuad = new CTexturedQuad();
		}
		*myQuad = *aCSprite.myQuad;
	}
	return *this;
}


void Tga2D::CSprite::InternalInit(const char* aTexturePath)
{
	if (myQuad)
	{
		SAFE_DELETE(myQuad);
	}

	myQuad = new CTexturedQuad();
	myQuad->Init(aTexturePath);
	
}


void Tga2D::CSprite::Init(const char* aTexturePath)
{
	InternalInit(aTexturePath);
}

void Tga2D::CSprite::Render()
{
	if (!myShouldRender)
	{
		return;
	}
	if (!myIsPartOfbatch)
	{
		myQuad->Render();
	}	
	myShouldRenderBatched = true;
}

void Tga2D::CSprite::SetPosition(const VECTOR2F& aPosition)
{
	myQuad->myPosition = aPosition;
}

const VECTOR2F& Tga2D::CSprite::GetPosition() const
{
	return myQuad->myPosition;
}

void Tga2D::CSprite::SetPivot(const VECTOR2F& aPivot)
{
	myQuad->myPivot = aPivot;
}

const VECTOR2F& Tga2D::CSprite::GetPivot() const
{
	return myQuad->myPivot;
}


void Tga2D::CSprite::SetTextureRect(float aStartX, float aStartY, float aEndX, float aEndY)
{
	myQuad->SetTextureRect(aStartX, aStartY, aEndX, aEndY);
}

STextureRext* Tga2D::CSprite::GetTextureRect()
{
	return &myQuad->GetTextureRect();
}

bool Tga2D::CSprite::HasSameTextureAs(CSprite* aSprite) const
{
	return myQuad->myTexture == aSprite->myQuad->myTexture;
}

const VECTOR2F& Tga2D::CSprite::GetUVOffset() const
{
	return myQuad->myUV;
}

void Tga2D::CSprite::SetUVOffset(const VECTOR2F& aUV)
{
	myQuad->SetUV(aUV);
}



void Tga2D::CSprite::SetColor(const CColor& aColor)
{
	myQuad->SetColor(aColor);
}

const CColor& Tga2D::CSprite::GetColor() const
{
	return myQuad->GetColor();
}


void Tga2D::CSprite::SetRotation( const float aRotationInRadians )
{
	myQuad->myRotation = aRotationInRadians;
}

float Tga2D::CSprite::GetRotation() const
{
	return myQuad->myRotation;
}

VECTOR2F Tga2D::CSprite::GetSize() const
{
	return myQuad->mySize * myQuad->mySizeMultiplier;
}

void Tga2D::CSprite::SetSizeRelativeToScreen(const VECTOR2F& aSize)
{
	myQuad->SetSizeRelativeToScreen(aSize);
	myHasChangedSize = true;
}

void Tga2D::CSprite::SetSizeRelativeToImage(const VECTOR2F& aSize)
{
	myQuad->SetSizeRelativeToImage(aSize);
	myHasChangedSize = true;
}

VECTOR2UI Tga2D::CSprite::GetImageSize() const
{
	return myQuad->myImageSize;
}

void Tga2D::CSprite::SetUVScale(VECTOR2F aScale)
{
	myQuad->myUVScale = aScale;
}

const VECTOR2F& Tga2D::CSprite::GetUVScale() const
{
	return myQuad->myUVScale;
}

bool Tga2D::CSprite::HasValidTexture() const
{
	return !myQuad->myTexture->myIsFailedTexture;
}

void Tga2D::CSprite::SetMap(const EShaderMap aMapType, const char* aPath)
{
	myQuad->SetMap(aMapType, aPath);
}

void Tga2D::CSprite::ReleaseAttachedTexture()
{
	CEngine::GetInstance()->GetTextureManager().ReleaseTexture(myQuad->myTexture);
}

Tga2D::CColor Tga2D::CSprite::GetPixelColor(unsigned short aX, unsigned short aY)
{
	return myQuad->myTexture->GetPixelColor(aX, aY);
}

const char* Tga2D::CSprite::GetImagePath() const
{
	if (myQuad && myQuad->myTexture)
	{
		return myQuad->myTexture->myPath.c_str();
	}
	return nullptr;
}

void Tga2D::CSprite::SetCustomShader(CCustomShader* aShader)
{
	if (myQuad)
	{
		myQuad->myCustomShader = aShader;
	}
}

void Tga2D::CSprite::SetBlendState(EBlendState aBlendState)
{
	if (myQuad)
	{
		myQuad->SetBlendState(aBlendState);
	}
}

void Tga2D::CSprite::SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode)
{
	if (myQuad)
	{
		myQuad->SetSamplerState(aFilter, aAddressMode);
	}
}

void Tga2D::CSprite::ClearBatchData()
{
	myShouldRenderBatched = false;
	myIsPartOfbatch = false;
}
