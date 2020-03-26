#pragma once
#include <vector>
#include <tga2d/math/vector2.h>
#include <tga2d/math/vector4.h>
#include <tga2d/math/color.h>

#ifndef _RETAIL
#define DEBUG_DRAW_LINE(aFrom, aTo, aColor) Tga2D::CDebugDrawer::DrawLine(aFrom, aTo, aColor);
#define DEBUG_DRAW_LINES(aFrom, aTo, aColor, aCount) Tga2D::CDebugDrawer::DrawLines(aFrom, aTo, aColor, aCount);
#define DEBUG_DRAW_ARROW(aFrom, aTo, aColor, aArrowHeadSize) Tga2D::CDebugDrawer::DrawArrow(aFrom, aTo, aColor, aArrowHeadSize);
#define DEBUG_DRAW_CIRCLE(aFrom, aRadius, aColor) Tga2D::CDebugDrawer::DrawCircle(aFrom, aRadius, aColor);
#else
#define DEBUG_DRAW_LINE(aFrom, aTo, aColor)
#define DEBUG_DRAW_LINES(aFrom, aTo, aColor, aCount)
#define DEBUG_DRAW_ARROW(aFrom, aTo, aColor, aArrowHeadSize)
#define DEBUG_DRAW_CIRCLE(aFrom, aRadius, aColor) 
#endif

#ifndef _RETAIL
#define MAXSAMPLES 100
namespace Tga2D
{
	class CText;
	class CLinePrimitive;
	class CSprite;
	class CPerformanceGraph;
	class CLineMultiPrimitive;
	class CDebugDrawer
	{
		friend class CErrorManager;
	public:
		CDebugDrawer(int aFlagset);
		~CDebugDrawer(void);
		void Init();
		void Update(float aTimeDelta);
		void Render();
		static void DrawLine(VECTOR2F aFrom, VECTOR2F aTo, CColor aColor = CColor(1, 1, 1, 1));
		static void DrawLines(VECTOR2F* aFrom, VECTOR2F* aTo, CColor* aColor, unsigned int aCount);

		static void DrawArrow(VECTOR2F aFrom, VECTOR2F aTo, CColor aColor = CColor(1, 1, 1, 1), float aArrowHeadSize = 1.0f);
		static void DrawCircle(VECTOR2F aPos, float aRadius, CColor aColor = CColor(1, 1, 1, 1));
		
	private:
		void ShowErrorImage();
		double CalcAverageTick(int newtick);
		static CLinePrimitive *myLineBuffer;
		static CLineMultiPrimitive* myLineMultiBuffer;
		static int myNumberOfRenderedLines;
		static int myMaxLines;
		CText* myFPS; 
		CText* myMemUsage;
		CText* myDrawCallText;
		CText* myCPUText;
		CText* myErrorsText;
		CSprite* myErrorSprite;
		
		int tickindex;
		int ticksum;
		int ticklist[MAXSAMPLES];
		float myShowErrorTimer;

		int myDebugFeatureSet;
		CPerformanceGraph *myPerformanceGraph;
		unsigned short myRealFPS;
		unsigned long myRealFPSAvarage;
	};
}
#else
namespace Tga2D
{
	class CDebugDrawer
	{
	public:
		CDebugDrawer(int /*aFlagset*/){}
		~CDebugDrawer(){}
		void Init(){}
		void Update(float /*aTimeDelta*/){}
		void Render(){}
		void DrawLine(VECTOR2F /*aFrom*/, VECTOR2F /*aTo*/, Vector4f /*aColor*/){}
		void DrawLines(VECTOR2F* /*aFrom*/, VECTOR2F* /*aTo*/, CColor* /*aColor*/, unsigned int /*aCount*/){}

		void DrawArrow(VECTOR2F /*aFrom*/, VECTOR2F /*aTo*/, Vector4f /*aColor*/, float /*aArrowHeadSize*/){}
		static void DrawArrow(VECTOR2F /*aFrom*/, VECTOR2F /*aTo*/, CColor /*aColor*/, float /*aArrowHeadSize*/){}
		void DrawCircle(VECTOR2F /*aPos*/, float /*aRadius*/, CColor /*aColor*/){}
		void ShowErrorImage(){}
	private:

	};
}

#endif