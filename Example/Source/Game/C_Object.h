#pragma once
#include "../TShared/Sh_Object.h"

class CNetworkPostMaster;

namespace Tga2D
{
	class CSprite;
}

class C_Object final : public Sh_Object
{
	public:
	C_Object(VECTOR2F aPos, CNetworkPostMaster* aPostMaster);

	~C_Object() override = default;
	void Init();
	void Update(float aDelta) override;
private:
	CNetworkPostMaster* myPostMaster;
	Tga2D::CSprite* mySprite =  nullptr;
	float myAngle = 0;
};

