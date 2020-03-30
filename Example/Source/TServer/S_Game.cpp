#include "pch.h"
#include "S_Game.h"
#include <tga2d/engine.h>
#include <tga2d/error/error_manager.h>
#include <thread>
#include "tga2d/text/text.h"
#include "imgui.h"
#include "../TShared/gsl/gsl_util"
#include "../TShared/NetworkUtils.h"

using namespace std::placeholders;
// ReSharper disable CppInconsistentNaming
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND aHWnd, UINT aMsg, WPARAM aWParam, LPARAM aLParam);
// ReSharper restore CppInconsistentNaming

// ReSharper disable once CppMemberFunctionMayBeStatic
LRESULT S_Game::WinProc(const HWND aHWnd, const UINT aMessage, const WPARAM aWParam, const LPARAM aLParam)  // NOLINT(misc-misplaced-const)
{
	//ourInputHandler.UpdateEvents(message, wParam, lParam);
	if (ImGui_ImplWin32_WndProcHandler(aHWnd, aMessage, aWParam, aLParam))
	{
		return true;
	}

	if (aMessage == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return 0;
}




S_Game::S_Game()
{
}

S_Game::~S_Game()
{
}

bool S_Game::Init(const std::wstring& /*aVersion*/, HWND /*aHWND*/, int /*aX*/, int /*aY*/)
{
	Tga2D::SEngineCreateParameters createParameters;

	createParameters.myInitFunctionToCall = std::bind(&S_Game::InitCallBack, this);
	createParameters.myWinProcCallback = std::bind(&S_Game::WinProc, this, _1, _2, _3, _4);
	createParameters.myUpdateFunctionToCall = std::bind(&S_Game::UpdateCallBack, this);
	createParameters.myApplicationName = L"TGA 2D Server ";
	createParameters.myWindowPosX = 512;
	createParameters.myWindowPosY = 400;
	createParameters.myWindowWidth = 1920 / 2;
	createParameters.myWindowHeight = 1080 / 2;
	createParameters.myClearColor = { 1,0,0,1 };
	createParameters.myEnableVSync = true;
	createParameters.myActivateDebugSystems = Tga2D::eDebugFeature_Fps |
		Tga2D::eDebugFeature_Mem |
		Tga2D::eDebugFeature_Drawcalls |
		Tga2D::eDebugFeature_Cpu |
		Tga2D::eDebugFeature_Filewatcher |
		Tga2D::eDebugFeature_OptimiceWarnings;

	if (!Tga2D::CEngine::Start(createParameters))
	{
		ERROR_PRINT("Fatal error! Engine could not start!");
		system("pause");
		return false;
	}

	// End of program
	return true;

}

void S_Game::InitCallBack()
{
	//myPostMaster.RegisterListener(ENetworkEventType::NetworkInfo, *this);


	myGameWorld.Init(&myPostMaster);
	
	std::thread serverThread([this]()
	{
		myServerMainInterface.StartServer(myPostMaster, myGameWorld);
		myServerMainInterface.CloseServer();
	});
	serverThread.detach();
}

void S_Game::UpdateCallBack()
{
	ImGui::Begin("Network statistiks");
	ImGui::PlotLines("Recieved data", myNetworkDataRecieved.data(), gsl::narrow<int>(myNetworkDataRecieved.size()));
	ImGui::PlotLines("Sent data", myNetworkDataSent.data(), gsl::narrow<int>(myNetworkDataSent.size()));

	if (!myNetworkDataRecieved.empty())
	{
		const float bytes = myNetworkDataRecieved.back();
		if (bytes > 1000)
		{
			ImGui::Text("Recieved last second: %f%s", BytesToMB(gsl::narrow<int>(bytes)), " MB");
		}
		else
		{
			ImGui::Text("Recieved last second: %f%s", bytes, " Bytes");
		}

	}


	if (!myNetworkDataSent.empty())
	{
		const float bytes = myNetworkDataSent.back();
		if (bytes > 1000)
		{
			ImGui::Text("Sent last second: %f%s", BytesToMB(gsl::narrow<int>(bytes)), " MB");
		}
		else
		{
			ImGui::Text("Sent last second: %f%s", bytes, " Bytes");
		}
	}

	ImGui::End();

	ImGui::ShowDemoWindow();

	myGameWorld.Update(Tga2D::CEngine::GetInstance()->GetDeltaTime());
	//ourInputHandler.FinalizeUpdate();

	myPostMaster.DispatchScheduledEvents();
}

void S_Game::RecieveMessage(const SNetworkEvent* aEvent)
{
	//if (aEvent->myType == ENetworkEventType::NetworkInfo)
	//{
	//	myNetworkDataRecieved.push_back(gsl::narrow<float>(aEvent->u_myNetworkData.myRecievedDataLastSecond));
	//	if (myNetworkDataRecieved.size() > 20)
	//		myNetworkDataRecieved.erase(myNetworkDataRecieved.begin());

	//	myNetworkDataSent.push_back(gsl::narrow<float>(aEvent->u_myNetworkData.mySentDataLastSecond));
	//	if (myNetworkDataSent.size() > 20)
	//		myNetworkDataSent.erase(myNetworkDataSent.begin());
	//}
}