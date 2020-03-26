#include "stdafx.h"
#include "primitives/line_primitive.h"
#include "engine.h"
#include "render/Renderer.h"

using namespace Tga2D;

CLinePrimitive::CLinePrimitive(void)
{
}


CLinePrimitive::~CLinePrimitive(void)
{
}

void Tga2D::CLinePrimitive::Render()
{
	CEngine::GetInstance()->GetRenderer().AddToRender(this);
}

void Tga2D::CLineMultiPrimitive::Render()
{
	CEngine::GetInstance()->GetRenderer().AddToRender(this);
}

void Tga2D::CLineMultiPrimitive::Draw(CDirectEngine* aRenderer)
{
	aRenderer->DoDraw(this);
}
