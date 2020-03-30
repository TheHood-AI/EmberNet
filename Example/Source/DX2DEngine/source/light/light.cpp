#include "stdafx.h"
#include "light/light.h"
#include "engine.h"
#include "light/light_manager.h"

using namespace Tga2D;

CLight::CLight()
	:myIntensity(100.0f)
{
	myPosition.Set(0.5f, 0.5f); // Normalized
	myColor.Set(1, 0, 0, 1); // Normalized
	myIntensity = 1; // Multiplier
	myFallOff = 1; // Pixel space
}


CLight::~CLight()
{
}

void Tga2D::CLight::Render()
{
	CEngine::GetInstance()->GetLightManager().AddLightToRender(this);
}
