#include "pch.h"
#include "Sh_Character.h"

void Sh_Character::Move(const VECTOR2F& aDir, float aSpeed)
{
	myPos += aDir * aSpeed;
}


