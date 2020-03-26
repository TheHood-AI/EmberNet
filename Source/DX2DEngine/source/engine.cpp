#include "stdafx.h"

#include "engine.h"
#include "d3d/direct_3d.h"
#include "drawers/debug_drawer.h"
#include "error/error_manager.h"
#include "filewatcher/file_watcher.h"
#include "light/light_manager.h"
#include "render/renderer.h"
#include "text/text_service.h"
#include "texture/texture_manager.h"
#include "windows/windows_window.h"
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#pragma comment( lib, "user32.lib" )


using namespace Tga2D; 


CEngine* Tga2D::CEngine::myInstance = nullptr;




CEngine::CEngine( const SEngineCreateParameters& aCreateParameters)
: myWindow(nullptr)
, myDirect3D(nullptr)
, myRenderer(nullptr)
, myTextureManager(nullptr)
, myDebugDrawer(nullptr)
, myHwnd(nullptr)
, myHInstance(nullptr)
, myLightManager(nullptr)
, myCreateParameters(aCreateParameters)
, myRunEngine(true)
, myTotalTime(0.0f)
, myDeltaTime(0.0f)
, myShouldExit(false)
, myWantToUpdateSize(false)
{
	myWindowSize.x = myCreateParameters.myWindowWidth;
	myWindowSize.y = myCreateParameters.myWindowHeight;

	myRenderSize.x = myCreateParameters.myRenderWidth;
	myRenderSize.y = myCreateParameters.myRenderHeight;

	myTargetSize = myRenderSize;
	if (myCreateParameters.myTargetWidth > 0 && myCreateParameters.myTargetHeight > 0)
	{
		myTargetSize.x = myCreateParameters.myTargetWidth;
		myTargetSize.y = myCreateParameters.myTargetHeight;
	}
	

	myDebugDrawer = new CDebugDrawer(myCreateParameters.myActivateDebugSystems);

	myInitFunctionToCall = myCreateParameters.myInitFunctionToCall;
	myUpdateFunctionToCall = myCreateParameters.myUpdateFunctionToCall;
	
	myHwnd = myCreateParameters.myHwnd;
	myHInstance = myCreateParameters.myHInstance;
	myCreateParameters.myHInstance = myCreateParameters.myHInstance;
	myClearColor = myCreateParameters.myClearColor;

	myErrorManager = new CErrorManager();
	myErrorManager->AddLogListener(aCreateParameters.myLogFunction, aCreateParameters.myErrorFunction);
}


CEngine::~CEngine()
{
	SAFE_DELETE(myRenderer);
	SAFE_DELETE(myDebugDrawer);
	SAFE_DELETE(myTextService);
	SAFE_DELETE(myLightManager);
	SAFE_DELETE(myErrorManager);
	SAFE_DELETE(myFileWatcher);
	SAFE_DELETE(myTextureManager);
	SAFE_DELETE(myDirect3D);
	SAFE_DELETE(myWindow);
}


void Tga2D::CEngine::DestroyInstance()
{
	if (myInstance)
	{
		myInstance->Shutdown();
	}
	
	SAFE_DELETE(myInstance);
}


bool CEngine::Start(const SEngineCreateParameters& aCreateParameters)
{
	if (!myInstance)
	{
		myInstance = new CEngine(aCreateParameters);
		return myInstance->InternalStart();
	}
	else
	{
		ERROR_PRINT("%s", "DX2D::CEngine::CreateInstance called twice, thats bad.");
	}
	return false;
}

bool CEngine::InternalStart()
{
	IMGUI_CHECKVERSION();
	
	
	INFO_PRINT("%s", "#########################################");
	INFO_PRINT("%s", "---TGA 2D Starting, dream big and dare to fail---");
	myFileWatcher = new CFileWatcher();
	myWindow = new CWindowsWindow();
	if (!myWindow->Init(myWindowSize, myHwnd, &myCreateParameters, myHInstance, myCreateParameters.myWinProcCallback))
	{
		ERROR_PRINT("%s", "Window failed to be created!");
		return false;
	}

	myDirect3D = new CDirectEngine();
	if (!myDirect3D->Init(*this, myRenderSize, myCreateParameters))
	{
		ERROR_PRINT("%s", "D3D failed to be created!");
		myWindow->Close();
		return false;
	}

	myDirect3D->SetClearColor(myClearColor);

	myRenderer = new CRenderer();
	myTextureManager = new CTextureManager();
	myTextureManager->Init();

	myTextService = new CTextService();
	myTextService->Init();


	myLightManager = new CLightManager();
	CalculateRatios();

	if (myDebugDrawer)
	{
		myDebugDrawer->Init();
	}


	
	if (myInitFunctionToCall)
	{
		myInitFunctionToCall();
	}

	myStartOfTime = std::chrono::steady_clock::now();

    if( myUpdateFunctionToCall )
    {
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(*GetHWND());
		ImGui_ImplDX11_Init(GetDirect3D().GetDevice(), GetDirect3D().GetContext());
    	
        StartStep();
    	
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
        DestroyInstance();
    }

	return true;
}

void Tga2D::CEngine::Shutdown()
{
	if (myInstance)
	{
		myInstance->myRunEngine = false;
	}
}

void Tga2D::CEngine::StartStep()
{
	DoStep();
}

void Tga2D::CEngine::IdleProcess()
{

	if (!myDirect3D || myDirect3D->IsInitiated() == false)
	{
		return;
	}
	myTimer.Tick([&]()
	{
		myDeltaTime = static_cast<float>(myTimer.GetElapsedSeconds());
		myTotalTime += static_cast<float>(myTimer.GetElapsedSeconds());


		myDirect3D->PostRenderFrame();
		myTextureManager->Update();
		myTextService->Update();

		if (myRunEngine)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
		if (myRunEngine && myUpdateFunctionToCall)
		{
			myUpdateFunctionToCall();
		}


		if (myDebugDrawer)
		{
			myDebugDrawer->Update(myDeltaTime);
			myDebugDrawer->Render();
		}
		myRenderer->Update();

		if (myRunEngine)
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		myDirect3D->RenderFrame();

		myLightManager->PostFrameUpdate();

		if (myWantToUpdateSize)
		{
			UpdateWindowSizeChanges();
			myWantToUpdateSize = false;
		}
		myFileWatcher->FlushChanges();
	});
}

void Tga2D::CEngine::DoStep()
{
	MSG msg = {0};
	while (myRunEngine)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			if(msg.message == WM_QUIT)
			{
				INFO_PRINT("%s", "Exiting...");
				myRunEngine = false;
				break;
			}				

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		IdleProcess();
		
		std::this_thread::yield();
		
	}
}

void GetDesktopResolution(int& horizontal, int& vertical, HWND aHwnd)
{
	RECT r;
	GetClientRect(aHwnd, &r); //get window rect of control relative to screen
	horizontal = r.right - r.left;
	vertical = r.bottom - r.top;
}

void Tga2D::CEngine::UpdateWindowSizeChanges(bool aIgnoreAutoUpdate)
{
	if (myCreateParameters.myUseLetterboxAndPillarbox || aIgnoreAutoUpdate)
	{
		int horizontal = 0;
		int vertical = 0;
		GetDesktopResolution(horizontal, vertical, *myHwnd);

		// Both these values must be your real window size, so of course these values can't be static
		float screen_width = (float)horizontal;
		float screen_height = (float)vertical;

		// This is your target virtual resolution for the game, the size you built your game to
		float virtual_width = (float)myTargetSize.x;
		float  virtual_height = (float)myTargetSize.y;

		float targetAspectRatio = virtual_width / virtual_height;

		// figure out the largest area that fits in this resolution at the desired aspect ratio
		float  width = screen_width;
		float  height = (float)(width / targetAspectRatio);

		if (height > screen_height)
		{
			//It doesn't fit our height, we must switch to pillarbox then
			height = screen_height;
			width = (float)(height * targetAspectRatio);
		}

		// set up the new viewport centered in the backbuffer
		float  vp_x = (screen_width / 2.0f) - (width / 2.0f);
		float  vp_y = (screen_height / 2.0f) - (height / 2.0f);

		static float lastWidth = 0.0f;
		static float lastHeight = 0.0f;

		{
			lastWidth = width;
			lastHeight = height;
			SetViewPort(vp_x, vp_y, width, height, 0, 1, true);
		}
	}
	else
	{
		SetResolution(VECTOR2UI(myCreateParameters.myTargetWidth, myCreateParameters.myTargetHeight), false);
		SetViewPort(0, 0, myCreateParameters.myTargetWidth, myCreateParameters.myTargetHeight);
	}
}

float Tga2D::CEngine::GetWindowRatio() const
{
	return myWindowRatio;
}

float Tga2D::CEngine::GetWindowRatioInversed() const
{
	return myWindowRatioInversed;
}

VECTOR2F Tga2D::CEngine::GetWindowRatioVec() const
{
	return myWindowRatioVec;
}

VECTOR2F Tga2D::CEngine::GetWindowRatioInversedVec() const
{
	return myWindowRatioInversedVec;
}

void Tga2D::CEngine::SetResolution(const VECTOR2UI &aResolution, bool aAlsoSetWindowSize)
{
	myWindowSize = aResolution;
	if (aAlsoSetWindowSize)
	{
		myWindow->SetResolution(aResolution);
	}
	myDirect3D->SetResolution(aResolution);
	CalculateRatios();

}

void Tga2D::CEngine::CalculateRatios()
{
	float sizeX = static_cast<float>(myWindowSize.x);
	float sizeY = static_cast<float>(myWindowSize.y);
	if (sizeY > sizeX)
	{
		float temp = sizeX;
		sizeX = sizeY;
		sizeY = temp;
	}

	myWindowRatio = static_cast<float>(sizeX) / static_cast<float>(sizeY);
	myWindowRatioInversed = static_cast<float>(sizeY) / static_cast<float>(sizeX);
	
	myWindowRatioVec.x = 1.0f;
	myWindowRatioVec.y = 1.0f;
	myWindowRatioInversedVec.x = 1.0f;
	myWindowRatioInversedVec.y = 1.0f;
	if (sizeX >= sizeY)
	{
		myWindowRatioVec.y = myWindowRatio;
		myWindowRatioInversedVec.y = myWindowRatioInversed;
	}
	else
	{
		myWindowRatioVec.x = myWindowRatio;
		myWindowRatioInversedVec.x = myWindowRatioInversed;
	}
}

HWND* Tga2D::CEngine::GetHWND() const
{
	return myHwnd;
}


HINSTANCE Tga2D::CEngine::GetHInstance() const
{
	return myHInstance;
}

void Tga2D::CEngine::SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth, float aMaxDepth, bool aSetEngineResolution)
{
	if (myDirect3D)
	{
		if (aSetEngineResolution)
		{
			SetResolution(VECTOR2UI(static_cast<unsigned int>(aWidth), static_cast<unsigned int>(aHeight)), false);
		}	
		myDirect3D->SetViewPort(aTopLeftX, aTopLeftY, aWidth, aHeight, aMinDepth, aMaxDepth);
	}
}

void Tga2D::CEngine::SetFullScreen(bool aFullScreen)
{
	if (myDirect3D)
	{
		myDirect3D->SetFullScreen(aFullScreen);
	}
}

void Tga2D::CEngine::SetAmbientLightValue(float aAmbientLight)
{
	myLightManager->SetAmbience(aAmbientLight);
}

bool Tga2D::CEngine::IsDebugFeatureOn(eDebugFeatures aFeature) const
{
	const bool all = ((myCreateParameters.myActivateDebugSystems & eDebugFeature_All) != 0);
	if (all)
	{
		return true;
	}

	const bool specific = ((myCreateParameters.myActivateDebugSystems & aFeature) != 0);
	return specific;
}


//void Tga2D::CEngine::SetSampler(ESamplerType aType)
//{
//	if (myDirect3D)
//	{
//		myDirect3D->SetSampler(aType);
//	}
//}
//
//ESamplerType Tga2D::CEngine::GetSamplerType() const
//{
//	if (myDirect3D)
//	{
//		return myDirect3D->GetSamplerType();
//	}
//	return ESamplerType_Linear;
//}

void CEngine::SetRenderTarget(CSprite* aSpriteTarget)
{

	if (myDirect3D)
	{
		myDirect3D->SetRenderTarget(aSpriteTarget);
	}
}

bool CEngine::BeginFrame(const CColor &aClearColor)
{
	if (myShouldExit)
	{
		return false;
	}
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			INFO_PRINT("%s", "Exiting...");
			myShouldExit = true;
			return false;
		}
	}

    myFileWatcher->FlushChanges();
 
	myDirect3D->PostRenderFrame();
	myDirect3D->Clear(aClearColor);
    myTextureManager->Update();
    myTextService->Update();
	return true;
}


void CEngine::EndFrame( void )
{
	myTimer.Tick([&]()
	{
		myDeltaTime = static_cast<float>(myTimer.GetElapsedSeconds());
		myTotalTime += static_cast<float>(myTimer.GetElapsedSeconds());

		myRenderer->Update();

		if (myDebugDrawer)
		{
			myDebugDrawer->Update(myDeltaTime);
			myDebugDrawer->Render();
		}

		myDirect3D->RenderFrame();
		myLightManager->PostFrameUpdate();

		if (myWantToUpdateSize)
		{
			UpdateWindowSizeChanges();
			myWantToUpdateSize = false;
		}
	});

    std::this_thread::yield();
}
