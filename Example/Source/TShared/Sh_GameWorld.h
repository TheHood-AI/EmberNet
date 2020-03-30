#pragma once
#include <vector>
#include "GameObject.h"

class Sh_GameWorld
{
public:
	void Update(const float aDelta);
protected:
	std::vector<GameObject*> myGameObjects;
};

