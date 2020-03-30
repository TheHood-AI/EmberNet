#include "pch.h"
#include "S_Character.h"
#include <tga2d/sprite/sprite.h>

void S_Character::Init()
{
	mySprite = new Tga2D::CSprite("Sprites/Hacker.png");
}

void S_Character::Update(float aDelta)
{
	mySprite->SetPosition(myPos);
	mySprite->Render();
}
