#include "pch.h"
#include "NetworkPostMaster.h"
#include "Macros.h"

CNetworkPostMaster::~CNetworkPostMaster()
{
	if (myMutex.try_lock())
		myMutex.unlock();
}

void CNetworkPostMaster::DispatchScheduledEvents()
{
	{
		std::lock_guard<std::mutex> guard(myMutex);
		myEventQueueWork.swap(myEventQueueBuffer);
	}
	while (!myEventQueueWork.empty())
	{
		const SNetworkEvent event = myEventQueueWork.front();
		myEventQueueWork.pop();

		for (auto& listener : myRegisteredListeners[event.myType])
		{
			listener->RecieveMessage(&event);
		}
	}
}

void CNetworkPostMaster::RegisterListener(const ENetworkEventType aEventType, CNetworkEventListener& aEventListener)
{
	myRegisteredListeners[aEventType].push_back(&aEventListener);
}

void CNetworkPostMaster::RemoveListener(const ENetworkEventType aEventType, CNetworkEventListener& aEventListener)
{
	for (int i = myRegisteredListeners[aEventType].size() - 1; i >= 0; --i)
	{
		if (myRegisteredListeners[aEventType][i] == &aEventListener)
		{
			CYCLIC_ERASE(myRegisteredListeners[aEventType], i);
		}
	}
}

void CNetworkPostMaster::ScheduleEvent(const SNetworkEvent& aEvent)
{
	std::lock_guard<std::mutex> guard(myMutex);

	myEventQueueBuffer.push(aEvent);
}

void CNetworkPostMaster::SendEvent(const SNetworkEvent& aEvent)
{
	for (auto& listener : myRegisteredListeners[aEvent.myType])
	{
		listener->RecieveMessage(&aEvent);
	}
}