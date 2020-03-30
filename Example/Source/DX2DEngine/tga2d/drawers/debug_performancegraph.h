#pragma once
#include <vector>

#define DEBUG_PERFGRAPH_SAMPLES 500
namespace Tga2D
{
	class CText;
	class CLinePrimitive;
	class CSprite;
	class CCustomShape;
	class CDebugDrawer;
	class CPerformanceGraph
	{
	public:
		CPerformanceGraph(CDebugDrawer* aDrawer);
		~CPerformanceGraph(void);
		void Init(Tga2D::CColor& aBackgroundColor, Tga2D::CColor& aLineColor, const std::string& aText);
		void Render();

		void FeedValue(int aValue);
	private:
		CCustomShape* myBackGround;
		std::vector<int> myBuffer;
		CDebugDrawer* myDrawer;
		Tga2D::CColor myLineColor;
		CText* myText;

	};
}
