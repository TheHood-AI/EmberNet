/*
This class will store a texture bound to DX11
*/
#pragma once
#include "..\engine_defines.h"

#ifdef USE_VIDEO
namespace Tga2D
{
	enum VideoStatus
	{
		VideoStatus_Idle,
		VideoStatus_Playing,
		VideoStatus_ReachedEnd
	};
	class CVideo
	{
	public:
		CVideo();
		~CVideo();
		bool Init(const char* aPath, bool aPlayAudio = false);
		void Play(bool aLoop = false);
		void Pause();
		void Stop();

		void Update(float aDelta);

		/* Will return false if there was something wrong with the load of the video */
		bool Render();
		void Restart();
		
		class CSprite* GetSprite() const { return mySprite; }

		VECTOR2I GetVideoSize() const { return mySize; }
		VideoStatus GetStatus() const { return myStatus; }
	private:
		class CSprite* mySprite;
		class CVideoPlayer* myPlayer;

		struct ID3D11ShaderResourceView* myShaderResource;
		struct ID3D11Texture2D *myTexture;
		
		int *myBuffer;

		VECTOR2I mySize;

		float myUpdateTime;
		VideoStatus myStatus;
		bool myWantsToPlay;
		bool myIsLooping;

		int PowerSizeX;
		int PowerSizeY;
	};
}
#endif