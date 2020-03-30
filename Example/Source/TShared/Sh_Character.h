#pragma once
#include "Vector2.hpp"
#include "GameObject.h"

class Sh_Character : public GameObject
{
public:
	virtual ~Sh_Character() = default;
	Sh_Character() = default;
	virtual void Init() {};
	void Update(float aDelta) override = 0;
	void Move(const VECTOR2F& aDir, float aSpeed);

private:
};

