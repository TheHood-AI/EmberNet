#pragma once
#include "Sh_Character.h"
#include "NetworkPostMaster.h"

namespace Tga2D
{
	class CSprite;
}

class C_Character :
	public Sh_Character
{
public:
	~C_Character() = default;
	void Init() override;
	void Update(float aDelta) override;

	CNetworkPostMaster* myPostMaster;
	int myPeerID = 0;
private:
	Tga2D::CSprite* mySprite = nullptr;
};

