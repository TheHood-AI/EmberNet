#include "stdafx.h"
#include "light/light_manager.h"
#include "engine.h"
#include "render/Renderer.h"

using namespace Tga2D;

CLightManager::CLightManager()
{
	myAmbientLight = 1.0f;
}

CLightManager::~CLightManager()
{
}

void CLightManager::AddLightToRender(const CLight* aLight)
{
	if (myLightsToRender.size() >= NUMBER_OF_LIGHTS_ALLOWED)
	{
		ERROR_PRINT("%s%i%s", "We only allow ", NUMBER_OF_LIGHTS_ALLOWED, " lights at the same time and you are trying to push more");
		return;
	}
	myLightsToRender.push_back(aLight);
}

void Tga2D::CLightManager::PostFrameUpdate()
{
	myLightsToRender.clear();
}

void Tga2D::CLightManager::SetAmbience(float aAmbientLight)
{
	myAmbientLight = aAmbientLight;
}