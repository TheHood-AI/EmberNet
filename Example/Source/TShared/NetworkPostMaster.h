#pragma once
#include "NetworkPostMasterDataStructs.h"
#include <map>
#include <vector>
#include <queue>
#include <mutex>

struct SNetworkEvent
{
	SNetworkEvent() { u_myChar = 0; };
	ENetworkEventType myType = ENetworkEventType::COUNT;

	std::vector<unsigned int> myGameServerIDs;

	union
	{
		char u_myChar;
		SPlayerConnectedData u_myPlayerConnectedData;
		SPlayerDisConnectedData u_myPlayerDisconnectedData;
		SCreateObject u_myObjectData;
		SSetPlayerPos u_myPlayerPos;
	};

};

class CNetworkEventListener
{
public:
	virtual ~CNetworkEventListener() = default;
	virtual void RecieveMessage(const SNetworkEvent* aEvent) = 0;
};

class CNetworkPostMaster
{
public:
	~CNetworkPostMaster();
	void DispatchScheduledEvents();
	void RegisterListener(const ENetworkEventType aEventType, CNetworkEventListener& aEventListener);
	void RemoveListener(const ENetworkEventType aEventType, CNetworkEventListener& aEventListener);

	void ScheduleEvent(const SNetworkEvent& aEvent);
	void SendEvent(const SNetworkEvent& aEvent);

private:
	std::map<ENetworkEventType, std::vector<CNetworkEventListener*>> myRegisteredListeners;
	std::queue<SNetworkEvent> myEventQueueWork;
	std::queue<SNetworkEvent> myEventQueueBuffer;
	std::mutex myMutex;
};


