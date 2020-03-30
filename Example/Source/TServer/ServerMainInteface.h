#pragma once

class S_GameWorld;
class CServerMain;
class IServerMain
{
public:
	IServerMain();
	~IServerMain();
	void StartServer(CNetworkPostMaster& aPostMaster, S_GameWorld& aGameWorld);
	short GetRecievedBytePerSecond()const;
	void CloseServer();
private:
	CServerMain* myServer;
};

