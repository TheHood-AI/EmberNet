#include "stdafx.h"

#include "render/renderer.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "render/render_object.h"
#include "sprite/textured_quad.h"
#include "d3d/direct_3d.h"
#include "texture/texture.h"

using namespace Tga2D;

CRenderer::CRenderer()
	: myDXEngine(&CEngine::GetInstance()->GetDirect3D())
{

}


CRenderer::~CRenderer()
{
	myRenderedSpritesOfSameType.clear();
}

void CRenderer::AddToRender( CRenderObject* aObject )
{
	myDXEngine->Draw(aObject);

}

void Tga2D::CRenderer::AddToRender(CTexturedQuad* aObject)
{
	myDXEngine->Draw(aObject);

	if (CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_OptimiceWarnings))
	{
		OptimisationMap::iterator iter = myRenderedSpritesOfSameType.find(aObject->myTexture->myResource);
		if (iter == myRenderedSpritesOfSameType.end())
		{
			myRenderedSpritesOfSameType[aObject->myTexture->myResource].myPath = aObject->myTexture->myPath;
		}
		myRenderedSpritesOfSameType[aObject->myTexture->myResource].myCount++;	
	}
	
}

#define RENDER_WARN_COUNT 20
void Tga2D::CRenderer::Update()
{
	if (CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_OptimiceWarnings))
	{
		for (OptimisationMap::iterator iter = myRenderedSpritesOfSameType.begin(); iter != myRenderedSpritesOfSameType.end(); iter++)
		{
			if (iter->second.myCount > RENDER_WARN_COUNT)
			{
				if (!iter->second.myHasWarned)
				{
					INFO_TIP("%s%s%s%i%s%s%i%s%s", "Optimisation warning! Texture: ", iter->second.myPath.c_str(), " was rendered more than ", RENDER_WARN_COUNT, " this frame", "(", iter->second.myCount, ")!", " Consider making it a spritebath");
				}
				iter->second.myHasWarned = true;
				
			}
			iter->second.myCount = 0;
		}

	}
}

