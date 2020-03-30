#pragma once
#include <vector>

namespace Tga2D
{
	class CLight;
	class CLightManager
	{
	public:
		CLightManager();
		~CLightManager();
		void AddLightToRender(const CLight* aLight);

		void PostFrameUpdate();
		std::vector<const CLight*> myLightsToRender;

		const float GetAmbientLight() const { return myAmbientLight; }
		void SetAmbience(float aAmbientLight);
		unsigned short GetNumberOfLights() const { return static_cast<unsigned short>(myLightsToRender.size()); }
	private:

		float myAmbientLight;
	};
}

