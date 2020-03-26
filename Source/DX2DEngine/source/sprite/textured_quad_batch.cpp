#include "stdafx.h"

#include "sprite/textured_quad_batch.h"
#include "sprite/sprite.h"
#include "engine.h"
#include "texture/texture_manager.h"
#include "render/renderer.h"
#include "d3d/direct_3d.h"

using namespace Tga2D;

CTexturedQuadBatch::CTexturedQuadBatch(void)
{
	myEngine = CEngine::GetInstance();
	mySpriteBatch = nullptr;
	//mySize.Set(1, 1);
	myBlendState = EBlendState_Alphablend;
	myCustomShader = nullptr;
}


CTexturedQuadBatch::~CTexturedQuadBatch(void)
{
}

void Tga2D::CTexturedQuadBatch::Init(const char* aTexturePath)
{
	if (aTexturePath)
	{
		myTexturePath = aTexturePath;
		myTexture = myEngine->GetTextureManager().GetTexture(aTexturePath);
	}
	else
	{
		myTexture = myEngine->GetTextureManager().GetWhiteSquareTexture();
	}	
}

void Tga2D::CTexturedQuadBatch::Render(CSpriteBatch* aBatch)
{
	mySpriteBatch = aBatch;
	myEngine->GetRenderer().AddToRender(this);
}

void Tga2D::CTexturedQuadBatch::Draw(CDirectEngine* aRenderer)
{
	aRenderer->DoDraw(this);
}

void Tga2D::CTexturedQuadBatch::SetMap(EShaderMap aMapType, const char* aPath)
{
	myMaps[aMapType] = myEngine->GetTextureManager().GetTexture(aPath);
}
