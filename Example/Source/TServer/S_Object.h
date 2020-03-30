#pragma once
#include "GameObject.h"
#include "Sh_Object.h"

namespace Tga2D
{
	class CSprite;
}

class S_Object final : public Sh_Object
{
public:
	S_Object(VECTOR2F aPos, CNetworkPostMaster* aPostMaster);

	~S_Object() override = default;
	void Init();
	void Update(float aDelta) override;
private:
	CNetworkPostMaster* myPostMaster;
	Tga2D::CSprite* mySprite =  nullptr;
	float myAngle = 0;
};

