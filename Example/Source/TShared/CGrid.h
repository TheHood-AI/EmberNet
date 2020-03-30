#pragma once
#include "Vector2.hpp"
#include <vector>
#include "Sphere.hpp"
#include "../DX2DEngine/tga2d/engine.h"
#include "AABB3D.hpp"

struct SGridObject
{
	void SetPosition(const VECTOR2F& aPos)
	{
		myPos = aPos;
		mySphere.InitWithCenterAndRadius(VECTOR3F{ myPos.x, myPos.y, 0 }, 
			20.f / Tga2D::CEngine::GetInstance()->GetRenderSize().x * Tga2D::CEngine::GetInstance()->GetWindowRatioInversed());
	}

	VECTOR2F myPos;
	CommonUtilities::Sphere<float> mySphere;
};

struct GridCell
{
	std::vector<SGridObject*> myObjects;
	CommonUtilities::AABB3D<float> myBox;
};

class CGrid
{
public:
	void Update();
	void Draw()const;

	void AddToGrid(const VECTOR2F& aPos, SGridObject* aObject);
	std::vector<SGridObject*> GetAllObbjectsInCell(const VECTOR2F& aPos, const float aSize);
	std::vector<SGridObject*> GetObjectsInAdjacentCells(const VECTOR2F& aPos, const float aSize);
	std::vector<GridCell> myCells;
	float myCellWidth;
	float myCellHeight;
	unsigned char myGridWidth;
	unsigned char myGridHeight;
};