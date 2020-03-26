#include "pch.h"
#include "S_GameWorld.h"


#include "Macros.h"
#include "S_Character.h"
#include "tga2d/sprite/sprite.h"
#include "S_Object.h"
#include "Random.hpp"

S_GameWorld::~S_GameWorld()
{
}

void S_GameWorld::Init(CNetworkPostMaster* aPostMaster)
{
	aPostMaster->RegisterListener(ENetworkEventType::PlayerConnected, *this);
	aPostMaster->RegisterListener(ENetworkEventType::PlayerDisconnected, *this);
	aPostMaster->RegisterListener(ENetworkEventType::SetPlayerPos, *this);
	myPostMaster = aPostMaster;
}

unsigned objectUID = 0;
void S_GameWorld::Update(float aDelta)
{
	myCreateTimer -= aDelta;
	if(myCreateTimer < 0)
	{
		//myCreatedObjects++;
		//auto obj = new S_Object({CU::RandomFloat(0.1, 0.9), CU::RandomFloat(0.1, 0.9)}, myPostMaster);
		//obj->Init();
		//obj->SetID(objectUID);
		//myObjects.push_back(obj);
		//objectUID++;
		//SNetworkEvent event;
		//event.myType = ENetworkEventType::CreateObject;
		//event.u_myObjectData.myID = obj->GetID();
		//event.u_myObjectData.myPos = obj->GetPos();
		//myPostMaster->ScheduleEvent(event);
		//myCreateTimer = 2.f;
	}

	for(auto& object : myGameObjects)
	{
		object->Update(aDelta);
	}
	for(auto& object : myObjects)
	{
		object->Update(aDelta);
	}
}

void S_GameWorld::RecieveMessage(const SNetworkEvent* aEvent)
{
	if(aEvent->myType == ENetworkEventType::PlayerConnected)
	{
		auto ch = new S_Character();
		ch->Init();
		ch->SetPosition(aEvent->u_myPlayerConnectedData.myPos);
		ch->SetID(aEvent->u_myPlayerConnectedData.myID);
		myGameObjects.push_back(ch);
	}
	if(aEvent->myType ==  ENetworkEventType::PlayerDisconnected)
	{
		for (int i = myGameObjects.size(); i > 0; --i)
		{
			if (myGameObjects[i - 1]->GetID() == aEvent->u_myPlayerDisconnectedData.myID)
			{
				delete myGameObjects[i - 1];
				myGameObjects[i - 1] = nullptr;
				CYCLIC_ERASE(myGameObjects, i - 1);
				break;
			}
		}

	}
	if(aEvent->myType == ENetworkEventType::SetPlayerPos)
	{
		for(auto& obj : myGameObjects)
		{
			if(obj->GetID() == aEvent->u_myPlayerPos.myID)
			{
				obj->SetPosition(aEvent->u_myPlayerPos.myPos);
			}
		}
	}
}
