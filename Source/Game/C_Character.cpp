#include "stdafx.h"
#include "C_Character.h"
#include "tga2d/sprite/sprite.h"
#include "Game.h"

void C_Character::Init()
{
	mySprite = new Tga2D::CSprite("Sprites/Hacker.png");
}

void C_Character::Update(float aDelta)
{	
	if(CGame::ourInputHandler.IsMouseButtonHit(0) && myPeerID == myID)
	{
		auto pos = CGame::ourInputHandler.GetMousePos() / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().x);
			pos.y *= Tga2D::CEngine::GetInstance()->GetWindowRatio();
		myPos = pos;
		mySprite->SetPosition(myPos);
		SNetworkEvent event;
		event.myType = ENetworkEventType::SendPlayerPos;
		event.u_myPlayerPos.myID = myID;
		event.u_myPlayerPos.myPos = myPos;
		myPostMaster->ScheduleEvent(event);
	}

	mySprite->SetPosition(myPos);
	mySprite->Render();
}

