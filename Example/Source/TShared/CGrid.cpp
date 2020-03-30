#include "pch.h"
#include "CGrid.h"
#include "../DX2DEngine/tga2d//drawers/debug_drawer.h"
#include "Macros.h"

void CGrid::Update()
{
	int index = 0;
	for (auto& cell : myCells)
	{
		for (int i = static_cast<int>(cell.myObjects.size()) - 1; i >= 0; --i)
		{
			CommonUtilities::AABB3D<float> temp;
			temp.InitWithMinAndMax(VECTOR3F{ cell.myObjects[i]->myPos.x - cell.myObjects[i]->mySphere.GetRadius(), cell.myObjects[i]->myPos.y - cell.myObjects[i]->mySphere.GetRadius(), 0 },
				VECTOR3F{ cell.myObjects[i]->myPos.x + cell.myObjects[i]->mySphere.GetRadius(), cell.myObjects[i]->myPos.y + cell.myObjects[i]->mySphere.GetRadius(), 0 });

			const int minX = static_cast<int>(floor((temp.GetMin().x) / myCellWidth));
			const int minY = static_cast<int>(floor((temp.GetMin().y) / myCellHeight));

			const int x = max(minX, 0);
			const int y = max(minY, 0);
			if(x + (y * myGridWidth) != index)
			{ 
				myCells[x + (y * static_cast<int>(myGridWidth))].myObjects.push_back(cell.myObjects[i]);
				CYCLIC_ERASE(cell.myObjects, i);
			}
			
		}
		index++;
	}
}

void CGrid::Draw() const
{
	for (auto& cell : myCells)
	{
		Tga2D::CDebugDrawer::DrawLine({ cell.myBox.GetMin().x, cell.myBox.GetMin().y }, { cell.myBox.GetMin().x, cell.myBox.GetMax().y });
		Tga2D::CDebugDrawer::DrawLine({ cell.myBox.GetMin().x, cell.myBox.GetMax().y }, { cell.myBox.GetMax().x, cell.myBox.GetMax().y });
		Tga2D::CDebugDrawer::DrawLine({ cell.myBox.GetMax().x, cell.myBox.GetMax().y }, { cell.myBox.GetMax().x, cell.myBox.GetMin().y });
		Tga2D::CDebugDrawer::DrawLine({ cell.myBox.GetMax().x, cell.myBox.GetMin().y }, { cell.myBox.GetMin().x, cell.myBox.GetMin().y });

	}
}

void CGrid::AddToGrid(const CU::Vector2<float>& aPos, SGridObject* aObject)
{
	CommonUtilities::AABB3D<float> temp;
	temp.InitWithMinAndMax(VECTOR3F{ aObject->myPos.x - aObject->mySphere.GetRadius(), aObject->myPos.y - aObject->mySphere.GetRadius(), 0 },
		VECTOR3F{ aObject->myPos.x + aObject->mySphere.GetRadius(), aObject->myPos.y + aObject->mySphere.GetRadius(), 0 });

	const int minX = static_cast<int>(floor((temp.GetMin().x) / myCellWidth));
	const int minY = static_cast<int>(floor((temp.GetMin().y) / myCellHeight));
	const int maxX = static_cast<int>(floor((temp.GetMax().x) / myCellWidth));
	const int maxY = static_cast<int>(floor((temp.GetMax().y) / myCellHeight));

	for (int x = max(minX, 0); x <= maxX && x < myGridWidth; ++x)
	{
		for (int y = max(minY, 0); y <= maxY && y < myGridHeight; ++y)
		{
			myCells[x + (y * static_cast<int>(myGridWidth))].myObjects.push_back(aObject);
		}
	}
}

std::vector<SGridObject*> CGrid::GetAllObbjectsInCell(const CU::Vector2<float>& aPos, const float aSize)
{
	CommonUtilities::AABB3D<float> temp;
	const float size = aSize;
	temp.InitWithMinAndMax(VECTOR3F{ aPos.x - size, aPos.y - size, 0 }, VECTOR3F{ aPos.x + size, aPos.y + size, 0 });

	Tga2D::CDebugDrawer::DrawLine({ temp.GetMin().x, temp.GetMin().y }, { temp.GetMin().x, temp.GetMax().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMin().x, temp.GetMax().y }, { temp.GetMax().x, temp.GetMax().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMax().x, temp.GetMax().y }, { temp.GetMax().x, temp.GetMin().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMax().x, temp.GetMin().y }, { temp.GetMin().x, temp.GetMin().y });

	const int minX = static_cast<int>(floor((temp.GetMin().x) / myCellWidth));
	const int minY = static_cast<int>(floor(temp.GetMin().y / myCellHeight));
	const int maxX = static_cast<int>(floor((temp.GetMax().x) / myCellWidth));
	const int maxY = static_cast<int>(floor(temp.GetMax().y / myCellHeight));

	std::vector<SGridObject*> returnObjects;
	for (int x = max(minX, 0); x <= maxX && x < myGridWidth; ++x)
	{
		for (int y = max(minY, 0); y <= maxY && y < myGridHeight; ++y)
		{
			for (auto& obj : myCells[x + (y * static_cast<int>(myGridWidth))].myObjects)
			{
				if (std::find(returnObjects.begin(), returnObjects.end(), obj) == returnObjects.end())
				{
					returnObjects.push_back(obj);
				}
			}
		}
	}
	return returnObjects;
}

std::vector<SGridObject*> CGrid::GetObjectsInAdjacentCells(const VECTOR2F& aPos, const float aSize)
{
	CommonUtilities::AABB3D<float> temp;
	const float size = aSize;
	temp.InitWithMinAndMax(VECTOR3F{ aPos.x - size, aPos.y - size, 0 }, VECTOR3F{ aPos.x + size, aPos.y + size, 0 });

	Tga2D::CDebugDrawer::DrawLine({ temp.GetMin().x, temp.GetMin().y }, { temp.GetMin().x, temp.GetMax().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMin().x, temp.GetMax().y }, { temp.GetMax().x, temp.GetMax().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMax().x, temp.GetMax().y }, { temp.GetMax().x, temp.GetMin().y });
	Tga2D::CDebugDrawer::DrawLine({ temp.GetMax().x, temp.GetMin().y }, { temp.GetMin().x, temp.GetMin().y });

	const int minX = static_cast<int>(floor((temp.GetMin().x) / myCellWidth));
	const int minY = static_cast<int>(floor(temp.GetMin().y / myCellHeight));

	std::vector<SGridObject*> returnObjects;

	const int x = max(minX, 0);
	const int y = max(minY, 0);
	const int index = x + (y * myGridWidth);
	const int indexRight = index + 1;
	const int indexLeft = index- 1;
	const int indexUp = index + (y * myGridWidth);
	const int indexDown = index - ((y) * myGridWidth);

	if (indexUp < static_cast<int>(myCells.size()) && indexUp >= 0)
	{
		for (auto& obj : myCells[indexUp].myObjects)
		{
			returnObjects.push_back(obj);
		}
	}
	if (indexDown >= 0 && indexDown < static_cast<int>(myCells.size()))
	{
		for (auto& obj : myCells[indexDown].myObjects)
		{
			returnObjects.push_back(obj);
		}
	}
	if (indexRight < static_cast<int>(myCells.size()) && indexRight >= 0)
	{
		for (auto& obj : myCells[indexRight].myObjects)
		{
			returnObjects.push_back(obj);
		}
	}
	if (indexLeft >= 0 && indexLeft < static_cast<int>(myCells.size()))
	{
		for (auto& obj : myCells[indexLeft].myObjects)
		{
			returnObjects.push_back(obj);
		}
	}

	return returnObjects;
}