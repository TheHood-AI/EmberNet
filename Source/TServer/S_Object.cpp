#include "pch.h"
#include "S_Object.h"
#include "tga2d/sprite/sprite.h"
#include "NetworkPostMasterDataStructs.h"
#include "NetworkPostMaster.h"

S_Object::S_Object(CU::Vector2<float> aPos, CNetworkPostMaster* aPostMaster)
	: myPostMaster(aPostMaster)
{
	myPos = aPos;
}

void S_Object::Init()
{
	mySprite = new Tga2D::CSprite("Sprites/Seek.png");
	mySprite->SetPosition(myPos);
}


void S_Object::Update(float aDelta)
{
	if (mySprite)
	{
		mySprite->SetPosition(myPos);
		mySprite->Render();
	}

	//myAngle += aDelta;
	//myPos.x = myPos.x + (cos(myAngle) / 10.f) * aDelta;
	//myPos.y = myPos.y + (sin(myAngle) / 10.f) * aDelta;

	//if (mySprite)
	//{
	//	mySprite->SetPosition(myPos);
	//	mySprite->Render();
	//}
	//SNetworkEvent event;
	//	event.u_myMoveObjectData.myID = myID;
	//	event.u_myMoveObjectData.myPos = myPos;
	//	event.myType = ENetworkEventType::MoveObject;
	//	myPostMaster->ScheduleEvent(event);
	//	mySendMsgTimer = myNetworkFrequency;
}
