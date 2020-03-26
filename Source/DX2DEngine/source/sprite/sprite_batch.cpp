#include "stdafx.h"

#include "sprite/sprite_batch.h"
#include "sprite/sprite.h"
#include "sprite/textured_quad.h"
#include "sprite/textured_quad_batch.h"
#include "texture/texture_manager.h"
#include "shaders/shader_common.h"

using namespace Tga2D;

CSpriteBatch::CSpriteBatch(bool aDeleteSpritesWhenDestructed)
	:myQuadBatch(nullptr)
	, myDeleteSpritesOnDestruct(aDeleteSpritesWhenDestructed)
	, mySprites(4)

{
	TinyGrowingArray<CSprite*> newSpriteAdder;
	mySprites.push_back(newSpriteAdder);
	myNextIndex = 0;
	myNextBatch = 0;
}


CSpriteBatch::~CSpriteBatch(void)
{
	if (myDeleteSpritesOnDestruct)
	{
		DeleteAll();
	}
	delete myQuadBatch;
}

void CSpriteBatch::Init(const char* aTexturePath)
{
	if (myQuadBatch)
	{
		INFO_PRINT("%s%s", "Initing an already inited batch! Texture: ", aTexturePath);
		return;
	}
	myQuadBatch = new CTexturedQuadBatch();
	myQuadBatch->Init(aTexturePath);
}

bool CSpriteBatch::AddObject(CSprite* aSpriteObject)
{
	if (!aSpriteObject)
	{
		return false;
	}
	if (aSpriteObject->myShouldRenderBatched)
	{
		//Sprite already in a batch and ready to be rendered in one
		return true;
	}
	if (aSpriteObject->GetTexturedQuad()->myCustomShader)
	{
		INFO_PRINT("CSpriteBatch::AddObject() a sprite with a custom shader added. This will not work in a batch");
	}
	for (int containerIndex = 0; containerIndex < mySprites.size(); containerIndex++)
	{
		if (mySprites[containerIndex].IsFull() == true)
		{
			if (containerIndex == mySprites.size() - 1) // if last container is full, add a new one
			{
				TinyGrowingArray<CSprite*> newSpriteAdder(SPRITE_BATCH_COUNT);
				mySprites.push_back(newSpriteAdder);
			}
			continue;
		}
		aSpriteObject->myShouldRenderBatched = true;
		mySprites[containerIndex].push_back(aSpriteObject);
		return true;		
	}
	return false;
}


void Tga2D::CSpriteBatch::RemoveObject(CSprite* aSpriteObject, bool aAlsoDelete)
{	
	for (int containerIndex = 0; containerIndex < mySprites.size(); containerIndex++)
	{
		TinyGrowingArray<CSprite*>& sprites = mySprites[containerIndex];
		TinyGrowingArray<CSprite*>::iterator iter = sprites.begin();
		while (iter != sprites.end())
		{
			if (aSpriteObject == (*iter))
			{	
				iter = sprites.erase(iter);
				if (aAlsoDelete)
				{
					SAFE_DELETE(aSpriteObject);
				}
				else
				{
					aSpriteObject->myShouldRenderBatched = false;
				}
				
				if (sprites.size() == 0 && mySprites.size() > 1)
				{
					mySprites.Remove(containerIndex);
				}
				return;
			}
			else
			{
				++iter;
			}
		}
	}
}

void Tga2D::CSpriteBatch::Render()
{
	if (!myQuadBatch)
	{
		ERROR_PRINT("%s", "SpriteBatch not inited! Can not render!");
		return;
	}

	myQuadBatch->Render(this);
}

CSprite* Tga2D::CSpriteBatch::GetNext()
{
	if (myNextBatch > static_cast<int>(mySprites.size())-1)
	{
		return nullptr;
	}
	if (myNextIndex >  static_cast<int>(mySprites[myNextBatch].size()) - 1)
	{
		myNextIndex = 0;
		myNextBatch++;
		return GetNext();
	}

	CSprite* sprite = mySprites[myNextBatch][myNextIndex];
	myNextIndex++;
	return sprite;
}

void Tga2D::CSpriteBatch::ResetNext()
{
	myNextIndex = 0;
	myNextBatch = 0;
}

VECTOR2UI Tga2D::CSpriteBatch::GetImageSize()
{
	return myQuadBatch->myTexture->myImageSize;
}

const std::string Tga2D::CSpriteBatch::GetImagePath() const
{
	if (myQuadBatch && myQuadBatch->myTexture)
	{
		return myQuadBatch->myTexture->myPath;
	}

	return "";	
}

void Tga2D::CSpriteBatch::SetBlendState(EBlendState aBlendState)
{
	if (myQuadBatch)
	{
		myQuadBatch->SetBlendState(aBlendState);
	}
}

void Tga2D::CSpriteBatch::SetCustomShader(class CCustomShaderBatched* aShader)
{
	if (myQuadBatch)
	{
		myQuadBatch->myCustomShader = aShader;
	}
}

void Tga2D::CSpriteBatch::SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode)
{
	if (myQuadBatch)
	{
		myQuadBatch->SetSamplerState(aFilter, aAddressMode);
	}
}

void Tga2D::CSpriteBatch::ClearAll()
{
	for (int i = 0; i < mySprites.size(); i++)
	{
		TinyGrowingArray<CSprite*>& sprites = mySprites[i];
		for (CSprite* sprite : sprites)
		{
			sprite->myShouldRenderBatched = false;
		}
		sprites.clear();
	}
	mySprites.clear();
	TinyGrowingArray<CSprite*> newSpriteAdder;
	mySprites.push_back(newSpriteAdder);
}

void Tga2D::CSpriteBatch::SetMap(EShaderMap aMapType, const char* aPath)
{
	myQuadBatch->SetMap(aMapType, aPath);
}

void Tga2D::CSpriteBatch::DeleteAll()
{
	for (int i = 0; i < mySprites.size(); i++)
	{
		TinyGrowingArray<CSprite*>& sprites = mySprites[i];
		for (int j=0; j< sprites.size(); j++)
		{
			delete sprites[j];
		}
		sprites.clear();
	}
	mySprites.clear();
	ClearAll();
}