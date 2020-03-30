#pragma once
#include "../TShared/Sh_Character.h"

class S_Character : public Sh_Character
{
public:
	S_Character()=default;
	~S_Character()=default;

	void Init() override;
	void Update(float aDelta) override;
private:
	Tga2D::CSprite* mySprite;
};
