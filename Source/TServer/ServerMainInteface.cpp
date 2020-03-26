#include "pch.h"

#include "../TShared/CommonErrorHandling.h"
#include "ServerMainInteface.h"
#include "ServerMain.h"
#include "../TShared/NetworkPostMaster.h"

IServerMain::IServerMain()
	:myServer(nullptr)
{
}


IServerMain::~IServerMain()
{
}

void IServerMain::StartServer(CNetworkPostMaster& aPostMaster, S_GameWorld& aGameWorld)
{
	if (myServer)
	{
		PRINT_ERROR(L"Server already started!");
		return;
	}

	myServer = new CServerMain();

	myServer->SetPostMaster(&aPostMaster);
	std::thread serverUpdateThread([this]()
	{
		myServer->Start();
		
		while (true)
		{
			myServer->Update();

			Sleep(1000 / 60000);
		}

	});

	serverUpdateThread.detach();
	
	if (!myServer)
	{
		PRINT_ERROR(L"Out of memory");
	}
}

short IServerMain::GetRecievedBytePerSecond() const
{
	return 0;
}

void IServerMain::CloseServer()
{
	
}
