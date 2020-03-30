#include "stdafx.h"

#include "drawers/debug_performancegraph.h"
#include "drawers/debug_drawer.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "sprite/sprite.h"
#include "primitives/custom_shape.h"
#include "text/Text.h"

#include <windows.h>
#include <stdio.h>
#include <psapi.h>


using namespace Tga2D;

CPerformanceGraph::CPerformanceGraph(CDebugDrawer* aDrawer)
	:myDrawer(aDrawer)
	, myBackGround(nullptr)
{
}


CPerformanceGraph::~CPerformanceGraph(void)
{
	SAFE_DELETE(myBackGround);
	SAFE_DELETE(myText);
}

void CPerformanceGraph::Init(Tga2D::CColor& aBackgroundColor, Tga2D::CColor& aLineColor, const std::string& aText)
{
	myBackGround = new CCustomShape();
	myLineColor = aLineColor;
	const float sizeX = -0.4f;
	const float sizeY = 0.11f;
	const VECTOR2F position(1, -0.01f);


	myBackGround->AddPoint(VECTOR2F(position.x, position.y), aBackgroundColor);
	myBackGround->AddPoint(VECTOR2F(position.x + sizeX, position.y), aBackgroundColor);
	myBackGround->AddPoint(VECTOR2F(position.x + sizeX, position.y + sizeY), aBackgroundColor);

	myBackGround->AddPoint(VECTOR2F(position.x, position.y), aBackgroundColor);
	myBackGround->AddPoint(VECTOR2F(position.x, position.y + sizeY), aBackgroundColor);
	myBackGround->AddPoint(VECTOR2F(position.x + sizeX, position.y + sizeY), aBackgroundColor);
	myBackGround->BuildShape();


	myText = new Tga2D::CText("Text/arial.ttf");
	myText->SetText("--");
	myText->SetPosition(VECTOR2F(0.6f, 0.02f));
	myText->SetColor({ 1, 1, 1, 1.0f });
	myText->SetScale(1.0f);
	myText->SetText(aText);
}


void CPerformanceGraph::FeedValue(int aValue)
{
	if (myBuffer.size() > DEBUG_PERFGRAPH_SAMPLES)
	{
		myBuffer.erase(myBuffer.begin());
	}
	myBuffer.push_back(aValue);
}


void CPerformanceGraph::Render()
{
	if (!CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_FpsGraph))
	{
		return;
	}
	float startX = 0.6f;
	float startY = 0.10f;

	int maxVal = 0;
	int minVal = INT_MAX;

	for (unsigned int i = 0; i < myBuffer.size(); i++)
	{
		if (myBuffer[i] > maxVal)
		{
			maxVal = myBuffer[i];
		}
		if (myBuffer[i] < minVal)
		{
			minVal = myBuffer[i];
		}

	}


	float increaseX = 0.4f / (float)DEBUG_PERFGRAPH_SAMPLES;

	float lastY = startY;

	std::vector<VECTOR2F> theTos;
	std::vector<VECTOR2F> theFrom;
	std::vector<Tga2D::CColor> theColors;

	for (unsigned int i = 0; i < myBuffer.size(); i++)
	{
		float thisY = (float)myBuffer[i] / (float)maxVal;

		thisY = thisY / 10.0f;

		float incrWithI = increaseX * ((float)i+1);

		theTos.push_back(VECTOR2F(startX + incrWithI, lastY));
		theFrom.push_back(VECTOR2F((startX + incrWithI) + increaseX, thisY));
		theColors.push_back(myLineColor);
	
		
		lastY = thisY;
	}
	myBackGround->Render();
	if (theTos.size() > 0)
	{
		
		myDrawer->DrawLines(&theTos[0], &theFrom[0], &theColors[0], static_cast<int>(theTos.size()));
	}
	
	myText->Render();

}
