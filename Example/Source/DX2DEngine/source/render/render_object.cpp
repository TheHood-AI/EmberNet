#include "stdafx.h"

#include "render/render_object.h"
#include "d3d/direct_3d.h"


using namespace Tga2D;


CRenderObject::CRenderObject(void)
{
	myIsScreenSpace = false;
	myBlendState = EBlendState::EBlendState_Alphablend;
	mySamplerFilter = ESamplerFilter::ESamplerFilter_Bilinear;
	mySamplerAddressMode = ESamplerAddressMode::ESamplerAddressMode_Clamp;
}


CRenderObjectSprite::CRenderObjectSprite()
		:myRotation(0)
		,myColor(1, 1, 1, 1)
		,myPivot(0, 0)
		,mySize(1, 1)
		,myUV(0, 0)
		,myUVScale(1.0f, 1.0f)
		,mySizeMultiplier(1, 1)
{
}

void CRenderObjectSprite::Draw(CDirectEngine* aRenderer)
{
	aRenderer->DoDraw(this);
}

CRenderObjectLine::CRenderObjectLine()
	:myFromPosition(0.0f, 0.0f),myToPosition(0.0f, 0.0f),myColor(1, 0, 0, 1)
{

}

void CRenderObjectLine::Draw(CDirectEngine* aRenderer)
{
	aRenderer->DoDraw(this);
}

Tga2D::CRenderObjectCustom::CRenderObjectCustom()
{
	myPosition.Set(0, 0);
}

void Tga2D::CRenderObjectCustom::Draw(CDirectEngine* aRenderer)
{
	aRenderer->DoDraw(this);
}
