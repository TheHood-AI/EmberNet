#pragma once
#include <unordered_map>
#include <functional>

namespace Tga2D
{
	enum class AnimationEventType 
	{
		SP_ANIMATION_START, SP_ANIMATION_INTERRUPT, SP_ANIMATION_END, SP_ANIMATION_COMPLETE, SP_ANIMATION_DISPOSE, SP_ANIMATION_EVENT
	};

	using anim_callback_function = std::function<void(int, std::string, AnimationEventType)>;
	using anim_callback_function_event = std::function<void(int, std::string, std::string,int, float, std::string)>;

	class CSpineAnimation_Internal;
	class CSpineAnimation 
	{
	public:
		CSpineAnimation();
		~CSpineAnimation();
		void Init(const char* aAtlasPath, const char* aBonePath);
		void Update(float aTimeDelta);
		void Render();

		/* Render all bones and connections*/
		void RenderDebug();

		void SetPosition(const VECTOR2F& aPosition) { myPosition = aPosition; }
		const VECTOR2F& GetPosition() const { return myPosition; }

		void SetRotation(const float aRotationInRadians) { myRotation = aRotationInRadians; }
		float GetRotation() const { return myRotation; }

		void SetScale(const VECTOR2F aScale) { myScale = aScale; }
		VECTOR2F GetScale() const { return myScale; }

		void SetFlipX(bool aFlipX) { myFlipX = aFlipX; }
		void SetFlipY(bool aFlipY) { myFlipY = aFlipY; }
		
		void SetColor(const Tga2D::CColor& aColor) { myColor = aColor; }
		const Tga2D::CColor& GetColor() const { return myColor; }

		/*For events that is build in such as start, end, interrupt*/
		void RegisterAnimationCallback(anim_callback_function aAnimCallback, int aRegisterID);

		/*For custom events that is build via the animation*/
		void RegisterAnimationEventCallback(anim_callback_function_event aAnimEvent, int aRegisterID);


		void RemoveCallback(int aRegisterID);
		void RemoveEventCallback(int aRegisterID);

		VECTOR2F GetBonePosition(const std::string& aBoneName) const;
		float GetBoneRotation(const std::string& aBoneName) const;

		/* Set the time it will take to go from an animation to another in seconds */
		void SetMixTimings(std::string aFrom, std::string aTo, float aDelay);

		/* This will play the next animation after the current one is finished */
		void AddAnimationToQueue(int aTrackIndex, std::string aName, bool aLoop, float aDelay);

		/* This will play the next animation instantly with blend times if set by:  SetMixTimings, otherwise instantly */
		void SetCurrentAnimation(int aTrackIndex, std::string aName, bool aLoop);

		const char* GetCurrentAnimation(int aTrackIndex);
		void ClearQueue();

		/*Internal*/
		void OnCallback(int aTrackIndex, std::string aName, AnimationEventType aAnimationEventType);
		void OnEventCallback(int aTrackIndex, std::string aName, std::string aEventName, int aIntVal, float aFloatVal, std::string aStringVal);

	private:

		Tga2D::CSpineAnimation_Internal* myInternalAnimation;
		VECTOR2F myPosition;
		VECTOR2F myScale;
		float myRotation;
		bool myFlipX;
		bool myFlipY;
		Tga2D::CColor myColor;

		std::unordered_map<int, anim_callback_function> myAnimCallbacks;
		std::unordered_map<int, anim_callback_function_event> myAnimationCallbackEvents;
		
	};
}