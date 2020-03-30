#include "stdafx.h"
#include "primitives/custom_shape.h"
#include "engine.h"
#include "render/Renderer.h"
#include "texture/texture_manager.h"

using namespace Tga2D;
CCustomShape::CCustomShape()
{
	myIndex = 0;
	myIsDirty = false;
	myTexture = nullptr;
	myBlendState = EBlendState_Alphablend;
	myHasTexture = false;
	SetSize({ 1, 1 });
}


CCustomShape::~CCustomShape()
{
}

void CCustomShape::Reset()
{
	myPoints.clear();
}

int CCustomShape::AddPoint(VECTOR2F aPoint, CColor aColor, VECTOR2F aUV)
{
	SCustomPoint point;
	point.myPosition = aPoint;
	point.myColor = aColor;
	point.myIndex = myIndex;
	point.myUV = aUV;
	myPoints.push_back(point);
	if (myPoints.size()%3 == 0)
	{
		myIndex++;
	}
	
	myIsDirty = true;
	return point.myIndex;
}

// Do this second
void CCustomShape::BuildShape()
{
#ifdef _DEBUG
	if (myPoints.size() %3 != 0)
	{
		INFO_PRINT("%s", "CCustomShape::BuildShape(): building shape with non even devided by 3 points, invalid! We need it to build triangles!");
		return;
	}

	if (myHasTexture && myTexture)
	{

		bool noUVsSet = true;
		for (SCustomPoint& point : myPoints)
		{
			if (point.myUV.x != 0 && point.myUV.y != 0)
			{
				noUVsSet = false;
			}
		}
		if (noUVsSet)
		{
			ERROR_PRINT("%s", "Warning! A custom shape that have a texture set have no valid UV coordinates, the shape will most likley not be shown. Set valid UV coordinates,")
		}
	}
#endif

	if (!myTexture)
	{
		myTexture = CEngine::GetInstance()->GetTextureManager().GetWhiteSquareTexture();
	}

	myIsDirty = false;
}

void Tga2D::CCustomShape::RemovePoint(int aIndex)
{
	for (size_t i = myPoints.size() - 1; i > 0; i--)
	{
		if (myPoints[i].myIndex == aIndex)
		{
			myPoints.erase(myPoints.begin() + i);
			myIsDirty = true;
		}
	}
}

void Tga2D::CCustomShape::Render()
{
	if (myIsDirty)
	{
		ERROR_PRINT("%s", "CCustomShape::Render() shape is dirty, please call CCustomShape::BuildShape() to verify shape");
		return;
	}
	CEngine::GetInstance()->GetRenderer().AddToRender(this);
}

void Tga2D::CCustomShape::SetTexture(const char* aPath)
{
	myTexture = CEngine::GetInstance()->GetTextureManager().GetTexture(aPath);
	myHasTexture = false;
	if (myTexture && !myTexture->myIsFailedTexture)
	{
		myHasTexture = true;
	}
}

