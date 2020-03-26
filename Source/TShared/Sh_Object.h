#pragma once
#include "GameObject.h"

class Sh_Object : public GameObject
{
public:
	Sh_Object() = default;
	virtual ~Sh_Object();

	virtual void Init() {};
	virtual void Render() {};
	virtual void Update(const float aDelta) override = 0;
};
