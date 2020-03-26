#pragma once

namespace Tga2D
{
	class CAudio
	{
	public:
		CAudio();
		~CAudio();
		void Init(const char* aPath, bool aIsLooping = false);
		void Play();
		void SetVolume(float aVolume);
		void SetPosition(VECTOR2F aPosition);

		// Stops playing the sample in two ways - by immediately and by
		// set loop flag to 0, therefore next loop is not come
		void Stop(bool aImmediately = true);

	private:
		static class AudioOut* myAudioOut;
		long myLoadedAudio;
		DWORD myChannel;
		VECTOR2F myPosition;
		float myVolume;
	};
}