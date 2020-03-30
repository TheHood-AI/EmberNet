#include "stdafx.h"
#include <tga2d/audio/audio.h>
#include <tga2d/audio/audio_out.h>
using namespace Tga2D;

#pragma comment(lib,"bass.lib")

AudioOut* CAudio::myAudioOut = nullptr;
CAudio::CAudio()
	:myLoadedAudio(-1),
	myChannel(0),
	myVolume(1.0f)
{
	myPosition.Set(0, 0);
}
CAudio::~CAudio()
{

}

void CAudio::Init(const char * aPath, bool aIsLooping)
{
	if (!myAudioOut)
	{
		myAudioOut = new Tga2D::AudioOut();
	}
	myLoadedAudio = myAudioOut->Load(aPath, aIsLooping);
	
}

void CAudio::Play()
{
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->Play(myLoadedAudio, myChannel, myVolume);
}

void Tga2D::CAudio::SetVolume(float aVolume)
{
	myVolume = aVolume;
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->SetVolume(myChannel, myVolume);
}

void Tga2D::CAudio::SetPosition(VECTOR2F aPosition)
{
	myPosition = aPosition;
	myAudioOut->SetPosition(myChannel, myPosition);
}

void CAudio::Stop(bool aImmediately)
{
	if (myLoadedAudio == -1)
	{
		return;
	}
	myAudioOut->Stop(myLoadedAudio, aImmediately);
}
