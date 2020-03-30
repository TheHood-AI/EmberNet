#include "stdafx.h"

#include "animation/spineanimation.h"
#include <tga2d/animation/SpineAnimation_internal.h>
#include <tga2d/primitives/custom_shape.h>
#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>
#include <fstream>

bool File_exist(std::string fileName)
{
	std::ifstream infile(fileName);
	bool isGood = infile.good();
	infile.close();
	return isGood;
}

using namespace Tga2D;
CSpineAnimation::CSpineAnimation()
{
	myPosition.Set(0, 0);
	myScale.Set(1, 1);
	myRotation = 0.0f;
	myFlipX = false;
	myFlipY = false;
	myColor.Set(1, 1, 1, 1);
	myInternalAnimation = nullptr;
}

Tga2D::CSpineAnimation::~CSpineAnimation()
{
	SAFE_DELETE(myInternalAnimation);
}


void callback(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event)
{
	const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;

	CSpineAnimation* classInstance = static_cast<CSpineAnimation*>(state->userData);
	if (!classInstance)
	{
		return;
	}

	if (type == ANIMATION_EVENT)
	{
		std::string sEvent = event->stringValue ? event->stringValue : "";
		classInstance->OnEventCallback(entry->trackIndex, animationName, event->data->name, event->intValue, event->floatValue, sEvent);
	}
	else
	{
		classInstance->OnCallback(entry->trackIndex, animationName, AnimationEventType(type));
	}

}

void Tga2D::CSpineAnimation::Init(const char* aAtlasPath, const char* aBonePath)
{
	if (!File_exist(aAtlasPath))
	{
		ERROR_PRINT("%s %s", "Failed to load resource: ", aAtlasPath);
		return;
	}

	if (!File_exist(aBonePath))
	{
		ERROR_PRINT("%s %s", "Failed to load resource: ", aBonePath);
		return;
	}
	myInternalAnimation = new CSpineAnimation_Internal(aAtlasPath, aBonePath);
	myInternalAnimation->timeScale = 1;

	Skeleton* skeleton = myInternalAnimation->mySkeleton;
	if (!skeleton)
	{
		return;
	}
	skeleton->flipX = false;
	skeleton->flipY = false;
	Skeleton_setToSetupPose(skeleton);

	skeleton->x = 0;
	skeleton->y = 0;

	Skeleton_updateWorldTransform(skeleton);

	myInternalAnimation->myAnimationState->userData = this;
	myInternalAnimation->myAnimationState->listener = callback;
	
}

void Tga2D::CSpineAnimation::Update(float aTimeDelta)
{
	if (!myInternalAnimation || !myInternalAnimation->mySkeleton)
	{
		return;
	}

	for (Tga2D::CCustomShape* shape : myInternalAnimation->myShapes)
	{
		shape->SetSize(myScale);
		shape->SetRotation(myRotation);
	}

	myInternalAnimation->mySkeleton->flipX = myFlipX == true ? 1 : 0;
	myInternalAnimation->mySkeleton->flipY = myFlipY == true ? 1 : 0;
	myInternalAnimation->myColor = myColor;
	myInternalAnimation->Update(aTimeDelta);
}

void Tga2D::CSpineAnimation::Render()
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->draw(myPosition, myScale);
}

void Tga2D::CSpineAnimation::RenderDebug()
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->DrawDebug();
}

void Tga2D::CSpineAnimation::RegisterAnimationCallback(anim_callback_function aAnimCallback, int aRegisterID)
{
	myAnimCallbacks[aRegisterID] = (aAnimCallback);
}

void Tga2D::CSpineAnimation::RegisterAnimationEventCallback(anim_callback_function_event aAnimEvent, int aRegisterID)
{
	myAnimationCallbackEvents[aRegisterID] = (aAnimEvent);
}

void Tga2D::CSpineAnimation::RemoveCallback(int aRegisterID)
{
	myAnimCallbacks.erase(aRegisterID);
}

void Tga2D::CSpineAnimation::RemoveEventCallback(int aRegisterID)
{
	myAnimationCallbackEvents.erase(aRegisterID);
}

VECTOR2F Tga2D::CSpineAnimation::GetBonePosition(const std::string& aBoneName) const
{
	for (int boneIndex = 0; boneIndex < myInternalAnimation->mySkeleton->bonesCount; ++boneIndex)
	{
		if (myInternalAnimation->mySkeleton->bones[boneIndex]->data->name == aBoneName)
		{
			VECTOR2UI winTargetSize = Tga2D::CEngine::GetInstance()->GetTargetSize();
			spBone* bone = myInternalAnimation->mySkeleton->bones[boneIndex];
			VECTOR2F bonePos = VECTOR2F(bone->worldX / (float)winTargetSize.x, bone->worldY / (float)winTargetSize.y);

			return  bonePos*myScale + myPosition;
		}
	}	

	ERROR_PRINT("%s %s", "GetBonePosition - no bone with name found: ", aBoneName.c_str());
	return { 0.f, 0.f };
}

float Tga2D::CSpineAnimation::GetBoneRotation(const std::string& aBoneName) const
{
	for (int boneIndex = 0; boneIndex < myInternalAnimation->mySkeleton->bonesCount; ++boneIndex)
	{
		spBone* bone = myInternalAnimation->mySkeleton->bones[boneIndex];
		if (bone->data->name == aBoneName)
		{	
			float combinedRotation = bone->arotation;
			for (spBone* parent = bone->parent; parent != nullptr; parent = parent->parent)
			{
				combinedRotation += parent->arotation;
			}
			return (combinedRotation / 360.f) * PI2;
		}	
	}

	ERROR_PRINT("%s %s", "GetBoneRotation - no bone with name found: ", aBoneName.c_str());
	return { _FMAX };
}

void Tga2D::CSpineAnimation::SetMixTimings(std::string aFrom, std::string aTo, float aDelay)
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->DoMixAnimation(aFrom, aTo, aDelay);
}

void Tga2D::CSpineAnimation::AddAnimationToQueue(int aTrackIndex, std::string aName, bool aLoop, float aDelay)
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->AddAnimationToQueue(aTrackIndex, aName, aLoop, aDelay);
}

void Tga2D::CSpineAnimation::SetCurrentAnimation(int aTrackIndex, std::string aName, bool aLoop)
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->SetCurrentAnimation(aTrackIndex, aName, aLoop);
}

const char* Tga2D::CSpineAnimation::GetCurrentAnimation(int aTrackIndex)
{
	if (!myInternalAnimation)
	{
		return nullptr;
	}
	return myInternalAnimation->GetCurrentAnimation(aTrackIndex);
}

void Tga2D::CSpineAnimation::ClearQueue()
{
	if (!myInternalAnimation)
	{
		return;
	}
	myInternalAnimation->ClearQueue();
}

void Tga2D::CSpineAnimation::OnCallback(int aTrackIndex, std::string aName, AnimationEventType aAnimationEventType)
{
	for (auto onCall : myAnimCallbacks)
	{
		onCall.second(aTrackIndex, aName, aAnimationEventType);
	}
}

void Tga2D::CSpineAnimation::OnEventCallback(int aTrackIndex, std::string aName, std::string aEventName, int aIntVal, float aFloatVal, std::string aStringVal)
{
	for (auto onCall : myAnimationCallbackEvents)
	{
		onCall.second(aTrackIndex, aName, aEventName, aIntVal, aFloatVal, aStringVal);
	}
}

