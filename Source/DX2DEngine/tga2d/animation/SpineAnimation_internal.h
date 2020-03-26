#pragma once

#ifndef SPINE_SFML_H_
#define SPINE_SFML_H_

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>

_SP_ARRAY_DECLARE_TYPE(spColorArray, spColor)

namespace Tga2D
{
	class CCustomShape;
}

namespace Tga2D
{
	class CSpineAnimation_Internal
	{
	public:
		Skeleton* mySkeleton;
		AnimationState* myAnimationState;
		float timeScale;
		spVertexEffect* vertexEffect;

		CSpineAnimation_Internal(const char* aAtlasPath, const char* aBonePath);
		~CSpineAnimation_Internal();

		void Update(float deltaTime);

		virtual void draw(const VECTOR2F& aPosition, const VECTOR2F& aScale/*sf::RenderTarget& target, sf::RenderStates states*/) const;
		void DrawDebug();

		void DoMixAnimation(std::string aFrom, std::string aTo, float aDelay);
		void AddAnimationToQueue(int aTrackIndex, std::string aName, bool aLoop, float aDelay);

		void ClearQueue();

		void SetCurrentAnimation(int aTrackIndex, std::string aName, bool aLoop);
		const char* GetCurrentAnimation(int aTrackIndex);
		void setUsePremultipliedAlpha(bool usePMA) { usePremultipliedAlpha = usePMA; };
		bool getUsePremultipliedAlpha() { return usePremultipliedAlpha; };
		CColor myColor;
		std::vector<Tga2D::CCustomShape*> myShapes;
	private:
		bool ownsAnimationStateData;
		float* myWorldVertices;
		spFloatArray* tempUvs;
		spColorArray* tempColors;
		spSkeletonClipping* myClipper;
		SkeletonData* mySkeletonData = nullptr;
		bool usePremultipliedAlpha;
		int myNumBones;
		spAnimationStateData* myStateData;
		Atlas* myAtlas = nullptr;

	};
}

#endif /* SPINE_SFML_H_ */
