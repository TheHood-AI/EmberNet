#include "stdafx.h"
#ifndef _RETAIL
#include "drawers/debug_drawer.h"
#include "drawers/debug_performancegraph.h"
#include "primitives/line_primitive.h"
#include "text/Text.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "render/renderer.h"
#include "sprite/sprite.h"
#include "light/light_manager.h"

#include <windows.h>
#include <stdio.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

using namespace Tga2D;

Tga2D::CLinePrimitive * Tga2D::CDebugDrawer::myLineBuffer = nullptr;
Tga2D::CLineMultiPrimitive* Tga2D::CDebugDrawer::myLineMultiBuffer = nullptr;
int Tga2D::CDebugDrawer::myNumberOfRenderedLines = 0;
int Tga2D::CDebugDrawer::myMaxLines = 10000;

CDebugDrawer::CDebugDrawer(int aFlagset)
{
	myDebugFeatureSet = aFlagset;
	tickindex = 0;
	ticksum = 0;
	myRealFPS = 0;
	myRealFPSAvarage = 0;
}


CDebugDrawer::~CDebugDrawer(void)
{
	SAFE_ARRAYDELETE(myLineBuffer);
	SAFE_DELETE(myFPS);
	SAFE_DELETE(myMemUsage);
	SAFE_DELETE(myDrawCallText);
	SAFE_DELETE(myCPUText);
	SAFE_DELETE(myPerformanceGraph);
	SAFE_DELETE(myLineMultiBuffer);
	SAFE_DELETE(myErrorsText);
	SAFE_DELETE(myErrorSprite);
}

void CDebugDrawer::Init()
{
	myLineBuffer = new CLinePrimitive[myMaxLines];

	const float theSize = 1.0f;
	myFPS = new Tga2D::CText("Text/arial.ttf", EFontSize_24);
	myFPS->SetText("--");
	myFPS->SetPosition(VECTOR2F(0, 0.035f));
	myFPS->SetColor({ 1, 1, 1, 1.0f });
	myFPS->SetScale(theSize);

	myMemUsage = new Tga2D::CText("Text/arial.ttf");
	myMemUsage->SetText("--");
	myMemUsage->SetPosition(VECTOR2F(0.0f, 0.06f));
	myMemUsage->SetColor({ 1, 1, 1, 1.0f });
	myMemUsage->SetScale(theSize);

	myDrawCallText = new Tga2D::CText("Text/arial.ttf");
	myDrawCallText->SetText("--");
	myDrawCallText->SetPosition(VECTOR2F(0.0f, 0.09f));
	myDrawCallText->SetColor({ 1, 1, 1, 1.0f });
	myDrawCallText->SetScale(theSize);

	myCPUText = new Tga2D::CText("Text/arial.ttf");
	myCPUText->SetText("--");
	myCPUText->SetPosition(VECTOR2F(0.0f, 0.12f));
	myCPUText->SetColor({ 1, 1, 1, 1.0f });
	myCPUText->SetScale(theSize);


	myErrorsText = new Tga2D::CText("Text/arial.ttf");
	myErrorsText->SetText("--");
	myErrorsText->SetPosition(VECTOR2F(0.0f, 0.15f));
	myErrorsText->SetColor({ 1, 0.7f, 0.7f, 1.0f });
	myErrorsText->SetScale(theSize);

	myErrorSprite = new CSprite("sprites/error.dds");
	myErrorSprite->SetPosition(VECTOR2F(0.9f, 0.0f));
	myErrorSprite->SetPivot(VECTOR2F(0.5f, 0.0f));
	myShowErrorTimer = 0.0f;


	myPerformanceGraph = new CPerformanceGraph(this);
	Tga2D::CColor bgColor(0, 1, 0, 0.4f);
	Tga2D::CColor lineColro(1, 1, 1, 1.0f);
	myPerformanceGraph->Init(bgColor, lineColro, "FPS spike detector");

	myLineMultiBuffer = new CLineMultiPrimitive();
}


ULONGLONG FixCPUTimings(const FILETIME &a, const FILETIME &b)
{
	LARGE_INTEGER la, lb;
	la.LowPart = a.dwLowDateTime;
	la.HighPart = a.dwHighDateTime;
	lb.LowPart = b.dwLowDateTime;
	lb.HighPart = b.dwHighDateTime;

	return la.QuadPart - lb.QuadPart;
}

float GetCPUUsage(FILETIME *prevSysKernel, FILETIME *prevSysUser,
	FILETIME *prevProcKernel, FILETIME *prevProcUser,
	bool firstRun = false)
{
	FILETIME sysIdle, sysKernel, sysUser;
	FILETIME procCreation, procExit, procKernel, procUser;

	if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser) ||
		!GetProcessTimes(GetCurrentProcess(), &procCreation, &procExit, &procKernel, &procUser))
	{
		// can't get time info so return
		return -1.;
	}

	// check for first call
	if (firstRun)
	{
		// save time info before return
		prevSysKernel->dwLowDateTime = sysKernel.dwLowDateTime;
		prevSysKernel->dwHighDateTime = sysKernel.dwHighDateTime;

		prevSysUser->dwLowDateTime = sysUser.dwLowDateTime;
		prevSysUser->dwHighDateTime = sysUser.dwHighDateTime;

		prevProcKernel->dwLowDateTime = procKernel.dwLowDateTime;
		prevProcKernel->dwHighDateTime = procKernel.dwHighDateTime;

		prevProcUser->dwLowDateTime = procUser.dwLowDateTime;
		prevProcUser->dwHighDateTime = procUser.dwHighDateTime;

		return -1.;
	}

	ULONGLONG sysKernelDiff = FixCPUTimings(sysKernel, *prevSysKernel);
	ULONGLONG sysUserDiff = FixCPUTimings(sysUser, *prevSysUser);

	ULONGLONG procKernelDiff = FixCPUTimings(procKernel, *prevProcKernel);
	ULONGLONG procUserDiff = FixCPUTimings(procUser, *prevProcUser);

	ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
	ULONGLONG procTotal = procKernelDiff + procUserDiff;

	return (float)((100.0 * procTotal) / sysTotal);
}


const int CONVERSION_VALUE = 1024;
void CDebugDrawer::Update(float aTimeDelta)
{
	if (!myDebugFeatureSet)
	{
		return;
	}

	std::string fpsText;
	
	fpsText.append("FPS: ");

	myRealFPS = static_cast<unsigned short>(1.0f / aTimeDelta);

	static float timeInter = 0;
	static int iterations = 0;
	iterations++;
	timeInter += aTimeDelta;
	myRealFPSAvarage += myRealFPS;
	static int avarageFPS = 0;
	if (timeInter >= 0.3f)
	{
		timeInter = 0;
		avarageFPS = myRealFPSAvarage / iterations;
		myRealFPSAvarage = 0;
		iterations = 0;
	}

	myPerformanceGraph->FeedValue(myRealFPS);

	fpsText.append(std::to_string(avarageFPS));
	myFPS->SetText(fpsText);

	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
		&memCounter,
		sizeof(memCounter));

	if (!result)
	{
		return;
	}

	SIZE_T memUsed = (memCounter.WorkingSetSize) / 1024;
	SIZE_T memUsedMb = (memCounter.WorkingSetSize) / 1024 / 1024;

	std::string mem = "Sys Mem: ";
	mem.append(std::to_string(memUsed));
	mem.append("Kb (");
	mem.append(std::to_string(memUsedMb));
	mem.append("Mb)");
	myMemUsage->SetText(mem);

	static FILETIME prevSysKernel, prevSysUser;
	static FILETIME prevProcKernel, prevProcUser;
	float usage = 0.0;

	// first call
	static bool firstTime = true;
	usage = GetCPUUsage(&prevSysKernel, &prevSysUser, &prevProcKernel, &prevProcUser, firstTime);
	firstTime = false;

	std::string cpuText;
	cpuText.append("CPU: ");
	cpuText.append(std::to_string(static_cast<int>(usage)));
	cpuText.append("%");
	myCPUText->SetText(cpuText);

	if (myShowErrorTimer > 0.0f)
	{
		if (myErrorSprite && myErrorSprite->HasValidTexture())
		{
			float theRandomShake = (((rand() % 100) / 100.0f) - 0.5f) * 0.06f;
			myErrorSprite->SetRotation(theRandomShake);
			myErrorSprite->SetColor(Tga2D::CColor(1, 1, 1, min(myShowErrorTimer, 1.0f)));
			myErrorSprite->Render();
		}
	}


	myShowErrorTimer -=  aTimeDelta;
	myShowErrorTimer = max(myShowErrorTimer, 0.0f);

	unsigned int errors = CEngine::GetInstance()->GetErrorManager().GetErrorsReported();
	if (errors > 0)
	{
		std::string errorsString;
		errorsString.append("Errors: ");
		errorsString.append(std::to_string(errors));
		myErrorsText->SetText(errorsString);
	}

	
}


double CDebugDrawer::CalcAverageTick(int newtick)
{
	ticksum -= ticklist[tickindex];  /* subtract value falling off */
	ticksum += newtick;              /* add new value */
	ticklist[tickindex] = newtick;   /* save new value so it can be subtracted later */
	if (++tickindex == MAXSAMPLES)    /* inc buffer index */
		tickindex = 0;

	/* return average */
	return((double)ticksum / MAXSAMPLES);
}

void CDebugDrawer::Render()
{
	float oldLight = Tga2D::CEngine::GetInstance()->GetLightManager().GetAmbientLight();
	Tga2D::CEngine::GetInstance()->GetLightManager().SetAmbience(1.0f);
	myPerformanceGraph->Render();
	for (int i=0; i< myNumberOfRenderedLines; i++)
	{
		myLineBuffer[i].Render();
	}
	myNumberOfRenderedLines = 0;

	if (myDebugFeatureSet != 0)
	{
		if (CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Fps))
		{
			myFPS->Render();
		}
		if (CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Mem))
		{
			myMemUsage->Render();
		}


		if (CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Cpu))
		{
			myCPUText->Render();
		}

	}

	unsigned int errors = CEngine::GetInstance()->GetErrorManager().GetErrorsReported();
	if (errors > 0)
	{
		myErrorsText->Render();
	}

	if (myDebugFeatureSet != 0 && CEngine::GetInstance()->IsDebugFeatureOn(eDebugFeature_Drawcalls))
	{
		std::string drawCalls;
		drawCalls.append("DrawCalls: ");
		int objCount = CEngine::GetInstance()->GetDirect3D().GetObjectRenderCount() + 1;
		drawCalls.append(std::to_string(objCount));
		myDrawCallText->SetText(drawCalls);
		myDrawCallText->SetColor({ 1, 1, 1, 1 });
		if (objCount > 300)
		{
			myDrawCallText->SetColor({ 1, 1, 0, 1 });
			if (objCount > 1000)
			{
				myDrawCallText->SetColor({ 1, 0, 0, 1 });
			}
		}

		myDrawCallText->Render();
	} 

	Tga2D::CEngine::GetInstance()->GetLightManager().SetAmbience(oldLight);

}

void Tga2D::CDebugDrawer::DrawLine(VECTOR2F aFrom, VECTOR2F aTo, CColor aColor)
{
	if (myNumberOfRenderedLines > myMaxLines)
	{
		return;
	}
	myLineBuffer[myNumberOfRenderedLines].myFromPosition = aFrom;
	myLineBuffer[myNumberOfRenderedLines].myToPosition = aTo;
	myLineBuffer[myNumberOfRenderedLines].myColor = aColor.AsVec4();
	myNumberOfRenderedLines++;
}

void Tga2D::CDebugDrawer::DrawLines(VECTOR2F* aFrom, VECTOR2F* aTo, CColor* aColor, unsigned int aCount)
{
	if (aCount <= 0)
	{
		return;
	}

	myLineMultiBuffer->myColor = aColor;
	myLineMultiBuffer->myFromPosition = aFrom;
	myLineMultiBuffer->myToPosition = aTo;
	myLineMultiBuffer->myCount = aCount;

	myLineMultiBuffer->Render();
}

void Tga2D::CDebugDrawer::ShowErrorImage()
{
	myShowErrorTimer = 4.0f;
}

void CDebugDrawer::DrawCircle(VECTOR2F aPos, float aRadius, CColor aColor)
{
	if (myNumberOfRenderedLines+32 > myMaxLines)
	{
		return;
	}
	const short circleResolution = 32;

	struct MultiLine
	{
		void Zero()
		{
			ZeroMemory(myColor, sizeof(CColor) * 256);
			ZeroMemory(myFromPosition, sizeof(VECTOR2F) * 256);
			ZeroMemory(myToPosition, sizeof(VECTOR2F) * 256);
		}
		CColor myColor[256];
		VECTOR2F myFromPosition[256];
		VECTOR2F myToPosition[256];
	};

	static MultiLine computedLineBuffer;
	computedLineBuffer.Zero();

	VECTOR2F to;
	bool first = false;
	float windowRatio = CEngine::GetInstance()->GetWindowRatio();
	int currentCount = 0;
	for (int i = 0; i < circleResolution+1; i++)
	{
		float angle = 2.0f * 3.14f * static_cast<float>(i) / static_cast<float>(circleResolution);
		
		if (!first)
		{
			first = true;
			to = VECTOR2F( aRadius*cos(angle) + aPos.x, (aRadius*windowRatio)*sin(angle) + aPos.y );
			continue;
		}

		computedLineBuffer.myColor[currentCount] = aColor;
		computedLineBuffer.myFromPosition[currentCount] = to;
		computedLineBuffer.myToPosition[currentCount] = VECTOR2F(aRadius*cos(angle) + aPos.x, (aRadius*windowRatio)*sin(angle) + aPos.y);

		to = computedLineBuffer.myToPosition[currentCount];
		currentCount++;		
	}


	myLineMultiBuffer->myColor = computedLineBuffer.myColor;
	myLineMultiBuffer->myFromPosition = computedLineBuffer.myFromPosition;
	myLineMultiBuffer->myToPosition = computedLineBuffer.myToPosition;
	myLineMultiBuffer->myCount = currentCount;

	myLineMultiBuffer->Render();

}

void CDebugDrawer::DrawArrow(VECTOR2F aFrom, VECTOR2F aTo, CColor aColor, float aArrowHeadSize)
{
	if (myNumberOfRenderedLines+3 > myMaxLines)
	{
		return;
	}

	float ration = CEngine::GetInstance()->GetWindowRatio();
	VECTOR2F direction = aTo - aFrom;
	direction = direction.Normalize();


	direction *= 0.04f * aArrowHeadSize;

	VECTOR2F theNormal = direction.Normal();
	theNormal.y *= ration;
	theNormal *= 0.2f * aArrowHeadSize;

	myLineBuffer[myNumberOfRenderedLines].myFromPosition = aFrom;
	myLineBuffer[myNumberOfRenderedLines].myToPosition = aTo;
	myLineBuffer[myNumberOfRenderedLines].myColor = aColor.AsVec4();
	myNumberOfRenderedLines++;

	myLineBuffer[myNumberOfRenderedLines].myFromPosition = aTo;
	myLineBuffer[myNumberOfRenderedLines].myToPosition = aTo - direction + theNormal;
	myLineBuffer[myNumberOfRenderedLines].myColor = aColor.AsVec4();
	myNumberOfRenderedLines++;

	myLineBuffer[myNumberOfRenderedLines].myFromPosition = aTo;
	myLineBuffer[myNumberOfRenderedLines].myToPosition = aTo - direction - theNormal;
	myLineBuffer[myNumberOfRenderedLines].myColor = aColor.AsVec4();
	myNumberOfRenderedLines++;

}
#endif _RETAIL