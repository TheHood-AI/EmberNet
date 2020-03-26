#include "stdafx.h"
#include "texture/Texture.h"
#include "DirectXTK/Inc/ScreenGrab.h"
#include "texture/texture_manager.h"

Tga2D::CTexture::CTexture()
{
	myPath = "undefined";
	myResource = nullptr;
	myIsFailedTexture = false;
	myVideoMemeorySize = 0;
	myRenderTarget = nullptr;
}

Tga2D::CTexture::~CTexture()
{
	if (myIsFailedTexture || IsWhiteTexture())
	{
		return;
	}
	SAFE_RELEASE(myResource);
	myResource = nullptr;
	myPath = "undefined";
	myID = 0;
	SAFE_ARRAYDELETE(myPixelColors);
	SAFE_RELEASE(myRenderTarget);
}

Tga2D::CColor Tga2D::CTexture::GetPixelColor(unsigned short aX, unsigned short aY, bool aRefreshData)
{
	CColor retColor;

	if (aX >= myImageSize.x
		||
		aY >= myImageSize.y)
	{
		ERROR_PRINT("%s%i%s%i%s%i%s%i", "GetPixelColor out of bounds! You want to get a pixel from: X", aX, " Y", aY, " but the image is: X", myImageSize.x - 1, " Y", myImageSize.y - 1);
		return retColor;
	}
	if (aRefreshData)
	{
		SAFE_DELETE(myPixelColors);
		myHasGrabbedColorData = false;
	}
	if (!myPixelColors && myHasGrabbedColorData)
	{
		return retColor;
	}
	if (!myPixelColors && !myHasGrabbedColorData)
	{
		myPixelColors = GrabPixelColors();
		myHasGrabbedColorData = true;
		if (!myPixelColors)
		{
			ERROR_PRINT("%s%s", "GetPixelColor can only grab data from a .DDS with format: A8R8G8B8! File:", myPath.c_str());
			return retColor;
		}
	}

	unsigned int pixelPlace = aX + (aY * myImageSize.x);

	pixelPlace *= 4;

	retColor.myA = myPixelColors[pixelPlace + 3];
	retColor.myR = myPixelColors[pixelPlace + 2];
	retColor.myG = myPixelColors[pixelPlace + 1];
	retColor.myB = myPixelColors[pixelPlace + 0];


	return retColor;
}

bool Tga2D::CTexture::IsWhiteTexture()
{
	return this == CEngine::GetInstance()->GetTextureManager().GetWhiteSquareTexture();
}

BYTE* Tga2D::CTexture::GrabPixelColors()
{
	ID3D11Resource* resource = NULL;;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	if (myResource)
	{
		myResource->GetResource(&resource);
		myResource->GetDesc(&srvDesc);
	}
	else
	{
		return nullptr;
	}

	ID3D11Texture2D* tex = (ID3D11Texture2D*)resource;
	BYTE* bytes = nullptr;
	if (tex)
	{
		bytes = DirectX::GrabPixelData(CEngine::GetInstance()->GetDirect3D().GetContext(), tex);
	}


	return bytes;

}


Tga2D::CRendertarget::CRendertarget() : CTexture()
{
	myTarget = nullptr;
}

Tga2D::CRendertarget::~CRendertarget()
{
	SAFE_RELEASE(myTarget)
}

void Tga2D::CTexture::InitAsRenderTarget(CDirectEngine* aDirectEngine, VECTOR2F aSize)
{
	D3D11_TEXTURE2D_DESC bufferInfo;
	bufferInfo.Width = (unsigned int)aSize.x * CEngine::GetInstance()->GetTargetSize().x;
	bufferInfo.Height = (unsigned int)aSize.y * CEngine::GetInstance()->GetTargetSize().y;
	bufferInfo.MipLevels = 1;
	bufferInfo.ArraySize = 1;
	bufferInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferInfo.SampleDesc.Count = 1;
	bufferInfo.SampleDesc.Quality = 0;
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = aDirectEngine->GetDevice()->CreateTexture2D(&bufferInfo, nullptr, &texture);
	hr = aDirectEngine->GetDevice()->CreateRenderTargetView(texture, nullptr, &myRenderTarget);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s", "Render target could not be created");
	}

	if (myResource)
	{
		SAFE_RELEASE(myResource);
	}

	hr = aDirectEngine->GetDevice()->CreateShaderResourceView(texture, nullptr, &myResource);
	if (FAILED(hr))
	{
		return;
	}

}
