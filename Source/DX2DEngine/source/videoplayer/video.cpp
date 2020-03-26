#include "stdafx.h"
#include "videoplayer/video.h"
#include "videoplayer/videoplayer.h"

#ifdef USE_VIDEO

#include "sprite/sprite.h"
#include "d3d/direct_3d.h"
#include "sprite/textured_quad.h"
#include "texture/texture_manager.h"

using namespace Tga2D;
Tga2D::CVideo::CVideo()
	:myPlayer(nullptr)
{
	myBuffer = nullptr;
	myUpdateTime = 0.0f;
	myStatus = VideoStatus_Idle;
	myWantsToPlay = false;
	myTexture = nullptr;
	myShaderResource = nullptr;
	mySprite = nullptr;
	myIsLooping = false;
}

Tga2D::CVideo::~CVideo()
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myShaderResource);

	if (mySprite)
	{
		delete mySprite;
		mySprite = nullptr;
	}

	if (myBuffer)
	{
		delete[] myBuffer;
	}

	delete myPlayer;
	myPlayer = nullptr;
	
}

void Tga2D::CVideo::Play(bool aLoop)
{
	myWantsToPlay = true;
	myIsLooping = aLoop;
}

void Tga2D::CVideo::Pause()
{
	myWantsToPlay = false;
}

void Tga2D::CVideo::Stop()
{
	myStatus = VideoStatus_Idle;
	myWantsToPlay = false;
	myPlayer->Stop();
}

void Tga2D::CVideo::Restart()
{
	myWantsToPlay = true;
	myPlayer->RestartStream();
}
#include <functional>
bool CVideo::Init(const char* aPath, bool aPlayAudio)
{
	if (myPlayer)
	{
		return false;
	}

	myPlayer = new CVideoPlayer();
	if (myPlayer)
	{
		EVideoError error = myPlayer->Init(aPath, aPlayAudio);
		if (error == EVideoError_WrongFormat)
		{
			ERROR_PRINT("%s %s %s", "Could not load video: ", aPath, ". Wrong format?");
			return false;
		}
		else if (error == EVideoError_FileNotFound)
		{
			ERROR_PRINT("%s %s %s", "Could not load video: ", aPath, ". File not found");
			return false;
		}


	}

	myShaderResource = nullptr;

	mySprite = new CSprite(nullptr);

	if (!myPlayer->DoFirstFrame())
	{
		ERROR_PRINT("%s %s %s", "Video error: ", aPath, ". First frame not found?");
		return false;
	}
	
	mySize.x = myPlayer->myAVVideoFrame->width;
	mySize.y = myPlayer->myAVVideoFrame->height;

	PowerSizeX = (int)powf(2, ceilf(logf((float)mySize.x) / logf(2)));
	PowerSizeY = (int)powf(2, ceilf(logf((float)mySize.y) / logf(2)));



	myBuffer = new int[(PowerSizeX*PowerSizeY)];

	myStatus = VideoStatus_Playing;


	if (!myShaderResource)
	{
		D3D11_TEXTURE2D_DESC texture_desc;
		texture_desc.Width = PowerSizeX;
		texture_desc.Height = PowerSizeY;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;

		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage = D3D11_USAGE_DYNAMIC;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texture_desc.MiscFlags = 0;

		CEngine::GetInstance()->GetDirect3D().GetDevice()->CreateTexture2D(&texture_desc, nullptr, &myTexture);
		CEngine::GetInstance()->GetDirect3D().GetDevice()->CreateShaderResourceView(myTexture, NULL, &myShaderResource);

		mySprite->GetTexturedQuad()->myTexture->myResource = myShaderResource;

		VECTOR2F texSize = VECTOR2F(static_cast<float>(mySize.x), static_cast<float>(mySize.y));

		VECTOR2F windowSize;
		windowSize.x = static_cast<float>(CEngine::GetInstance()->GetTargetSize().y);
		windowSize.y = static_cast<float>(CEngine::GetInstance()->GetTargetSize().y);
		texSize = texSize / windowSize;
		
		mySprite->SetSizeRelativeToScreen(VECTOR2F(texSize.x, texSize.y));

		float uvSizeX = (float)mySize.x / (float)PowerSizeX;
		float uvSizeY = (float)mySize.y / (float)PowerSizeY;

		mySprite->SetTextureRect(0, 0, uvSizeX, uvSizeY);
	}


	// Force one frame so we dont get a blanc video
	bool wantsToPlay = myWantsToPlay;
	myWantsToPlay = true;
	myUpdateTime = 0.0001f;
	Update(0);
	myUpdateTime = 0.0f;
	myWantsToPlay = wantsToPlay;

	return true;
}

void CVideo::Update(float aDelta)
{
	if (!myWantsToPlay || !myPlayer)
	{
		return;
	}
	myUpdateTime += aDelta;

	double fps = myPlayer->GetFps();

	myPlayer->Updateound();
	while (myUpdateTime >= 1.0f / fps)
	{
		if (myShaderResource && myTexture)
		{
			int status = myPlayer->GrabNextFrame();
			if (status < 0)
			{
				myStatus = VideoStatus_ReachedEnd;
				if (myIsLooping)
				{
					Restart();
				}
			}

			CDirectEngine& engine = CEngine::GetInstance()->GetDirect3D();
			D3D11_MAPPED_SUBRESOURCE  mappedResource;
			HRESULT result = engine.GetContext()->Map(myTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				return;
			}

			unsigned int* source = (unsigned int*)(mappedResource.pData);
			myPlayer->Update(source, PowerSizeX, PowerSizeY);
			engine.GetContext()->Unmap(myTexture, 0);
		}
		myUpdateTime -= 1.0f / static_cast<float>(fps);
	}
}

bool CVideo::Render()
{
	if (mySprite && myShaderResource)
	{
		mySprite->GetTexturedQuad()->myTexture->myResource = myShaderResource;
		mySprite->Render();
		return true;
	}
	return false;
}

#endif