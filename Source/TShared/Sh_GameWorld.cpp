#include "pch.h"
#include "Sh_GameWorld.h"

void Sh_GameWorld::Update(const float aDelta)
{
	for(auto& object : myGameObjects)
	{
		object->Update(aDelta);
	}
}
