#pragma once
#include "../NetworkLibrary/PeerWrapper.h"
#include "NetworkPostMaster.h"
#include "Timer.hpp"
struct SNetworkEvent;
class S_GameWorld;

class CServerMain : public CNetworkEventListener
{
public:
	CServerMain();
	~CServerMain();

	void Start();
	void Update();
	void SetPostMaster(CNetworkPostMaster* aPostMaster);
	
	void RecieveMessage(const SNetworkEvent* aEvent) override;
private:
	void PlayerConnect(char* someData, unsigned int aSIze);
	CNetworkPostMaster* myPostMaster = nullptr;
	CU::FrameTimer myTimer;
	float myCountTime = 0;
	EmberNet::IPeer myPeer;
	std::vector<CPlayerConnectedMessage> myGameData;
};


