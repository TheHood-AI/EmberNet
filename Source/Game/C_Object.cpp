#include "stdafx.h"
#include "C_Object.h"
#include "tga2d/sprite/sprite.h"

C_Object::C_Object(CU::Vector2<float> aPos, CNetworkPostMaster* aPostMaster)
	: myPostMaster(aPostMaster)
{
	myPos = aPos;
}

void C_Object::Init()
{
	mySprite = new Tga2D::CSprite("Sprites/Seek.png");
	mySprite->SetPosition(myPos);
}

void C_Object::Update(float aDelta)
{
	if (mySprite)
	{
		mySprite->SetPosition(myPos);
		mySprite->Render();
	}
}
