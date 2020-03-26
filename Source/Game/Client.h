#pragma once
#include "../NetworkLibrary/PeerWrapper.h"
#include "../TShared/NetworkPostMaster.h"

class CClient 
{
public:
	void Start();
	void Connect(const std::string& aConnectionAddres, const unsigned short aPort) const;
	void Update() const;
	void Send(const char* someData, const int aDataSize) const;
	int GetPeerID();
	void SetPostMaster(CNetworkPostMaster* aPostMaster);
private:
	void ConnectionConfirm(char* someData, unsigned int aSize);
	void PlayerConnected(char* someData, unsigned int aSize) const;
	void PlayerDisconnected(char* someData, unsigned int aSize) const;
	void CreateObject(char* someData, unsigned int aSize);
	
	EmberNet::IPeer myPeer;
	CNetworkPostMaster* myPostMaster = nullptr;
};

