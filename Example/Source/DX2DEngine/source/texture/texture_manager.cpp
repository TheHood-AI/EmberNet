#include "stdafx.h"

#include "texture/texture_manager.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "noise/perlin_noise.h"
#include "Math/color.h"
#include <d3d11.h>
#include <Gdiplus.h>
#include "engine_defines.h"
#include "filewatcher/file_watcher.h"
#include "xxh64_en.hpp"
#include "tgaloader.h"

using namespace Tga2D;


CTextureManager::CTextureManager(void)
	:myFailedResource(nullptr)
	,myDefaultNormalMapResource(nullptr)
	,myDirect3D(nullptr)
	, myNoiseResource(nullptr)
	, myNoiseTexture(nullptr)
{
}


CTextureManager::~CTextureManager(void)
{
	SAFE_RELEASE(myFailedResource);
	SAFE_RELEASE(myNoiseTexture);
	SAFE_RELEASE(myDefaultNormalMapResource);

	for (CTexture* texture : myResourceViews)
	{
		delete texture;
	}
	delete myWhiteSquareTexture;
}

void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ std::string aName)
{
#if defined(_DEBUG)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(aName.size()), aName.c_str());
#else
	resource;
	aName;
#endif
}

bool IsDDS(std::string aPath)
{
	if (aPath.substr(aPath.find_last_of(".") + 1) == "dds")
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool IsTarga(std::string aPath)
{
	if (aPath.substr(aPath.find_last_of(".") + 1) == "tga")
	{
		return true;
	}
	else
	{
		return false;
	}
}

CTexture* CTextureManager::GetTexture(const char* aTexturePath, bool aForceReload)
{
	if (!aTexturePath)
	{
		aTexturePath = "";
	}
	
	std::string toString = std::string(aTexturePath);
	const uint64_t hashedID = xxh64::hash(toString.c_str(), toString.length(), 0);
	std::vector<CTexture*>::iterator it = std::find_if(myResourceViews.begin(), myResourceViews.end(), [hashedID](const CTexture* s) { return s->myID == hashedID; });
	
	CTexture* loadedTexture = nullptr;
	if (it != myResourceViews.end())
	{
		loadedTexture = *it;
	}
	if (!aForceReload && loadedTexture)
	{
		CTexture* texture = *it;
		return texture;
	}

	ID3D11ShaderResourceView* resource = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(myDirect3D->GetDevice(), aTexturePath, nullptr, &resource);
	if (FAILED(hr))
	{
		if (!CAN_USE_OTHER_FORMATS_THAN_DDS)
		{
			ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds! ", aTexturePath);
			resource->Release();
			hr = E_FAIL;
		}
		else if (myDirect3D->IsOnSameThreadAsEngine(std::this_thread::get_id()))
		{
			hr = DirectX::CreateWICTextureFromFile(myDirect3D->GetDevice(), myDirect3D->GetContext(), Tga2D::ConvertStringToWstring(aTexturePath).c_str(), nullptr,
				&resource);
			if (FAILED(hr))
			{
				if (IsTarga(aTexturePath))
				{
					Tga32 targa;
					Tga32::Image* image = targa.Load(aTexturePath);
					if(image)
					{
						loadedTexture = CreateTextureFromTarga(image);
						if (loadedTexture)
						{
							resource = loadedTexture->myResource;
							hr = S_OK;
						}
					}
					else
					{
						ERROR_PRINT("%s %s", "This targa image is not supported, please use 32bit non compressed ", aTexturePath);
					}
				}
				else
				{
					ERROR_PRINT("%s %s", "This image format is forbidden! Use .dds, png, tga! ", aTexturePath);
				}
				
			}
		}
		else
		{
			INFO_PRINT("Trying to load a non-dds or a wierd format of dds on another thread than the engine. This is not supported, choose a correct dds format");
		}
		
	}

	if (!FAILED(hr))
	{
		VECTOR2F textureSize = GetTextureSize(resource, false);
		unsigned int x = static_cast<int>(textureSize.x);
		unsigned int y = static_cast<int>(textureSize.y);

		bool powerOfTwo = !(x == 0) && !(x & (x - 1));
		powerOfTwo &= !(y == 0) && !(y & (y - 1));
		if (!powerOfTwo && IsDDS(aTexturePath))
		{
			if (!CAN_USE_DDS_NOT_POWER_OF_TWO)
			{
				ERROR_PRINT("%s %s", "DDS is not power of two, this is forbidden! ", aTexturePath);
				resource->Release();
				hr = E_FAIL;
			}
			else
			{
				INFO_TIP("%s %s %i*%i", "DDS needs to be power of two!", aTexturePath, x, y);
			}
		}

	}
	
	if (!loadedTexture)
	{
		CEngine::GetInstance()->GetFileWatcher()->WatchFileChange(aTexturePath, std::bind(&Tga2D::CTextureManager::OnTextureChanged, this, std::placeholders::_1));
	}
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to load resource: ", aTexturePath);
		resource = myFailedResource;

		CTexture* newTexture = loadedTexture;
		if (!loadedTexture)
		{
			newTexture = new CTexture();
		}
		newTexture->myPath = aTexturePath;
		newTexture->myResource = resource;
		newTexture->myID = hashedID;
		SetDebugObjectName(newTexture->myResource, aTexturePath);
		newTexture->myIsFailedTexture = true;

		VECTOR2F texSize = GetTextureSize(resource);
		newTexture->mySize = VECTOR2F(0.3f, 0.3f);
		newTexture->myImageSize = VECTOR2F(512, 512);

		if (!loadedTexture)
		{
			myResourceViews.push_back(newTexture);
		}
		return newTexture;
	}
	if (resource)
	{

		CTexture* newTexture = loadedTexture;
		if (!loadedTexture)
		{
			newTexture = new CTexture();
		}
		newTexture->myPath = aTexturePath;
		newTexture->myID = hashedID;
		newTexture->myResource = resource;
		SetDebugObjectName(newTexture->myResource, aTexturePath);

		VECTOR2F texSize = GetTextureSize(resource);
		newTexture->mySize = texSize;
		
		newTexture->myImageSize = GetTextureSize(resource, false);

		size_t bitsPerPixel = DirectX::BitsPerPixel(GetTextureFormat(resource));
		newTexture->myVideoMemeorySize = (((newTexture->myImageSize.x * newTexture->myImageSize.y) * (int)bitsPerPixel) / 8) / 1000;

		if (!loadedTexture)
		{
			myResourceViews.push_back(newTexture);
		}

		
		return newTexture;
	}

	return nullptr;
}

void CTextureManager::OnTextureChanged(std::string aFile)
{
	std::string notWide = aFile;
	std::replace(notWide.begin(), notWide.end(), '\\', '/');
	for (int i=0; i< myResourceViews.size(); i++)
	{
		if (myResourceViews[i]->myPath.compare(notWide) != std::string::npos)
		{
			INFO_PRINT("%s%s", "Texture changed: ", notWide.c_str());
			GetTexture(notWide.c_str(), true);
			break;
		}
	}



}

CRendertarget* CTextureManager::GetRendertarget(const VECTOR2UI& aSize)
{
	HRESULT hr = S_OK;

	ID3D11Device* device = myDirect3D->GetDevice();
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = aSize.x;
	textureDesc.Height = aSize.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* temporaryTexture = nullptr;
	hr = device->CreateTexture2D(&textureDesc, nullptr, &temporaryTexture);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to create texture");
	}


	ID3D11ShaderResourceView* resource = nullptr;
	hr = device->CreateShaderResourceView(temporaryTexture, nullptr, &resource);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to create resource");
	}


	ID3D11RenderTargetView* target = nullptr;
	hr = device->CreateRenderTargetView(temporaryTexture, nullptr, &target);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to create target");
	}


	D3D11_TEXTURE2D_DESC depthDesc = textureDesc;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* temporaryDepth = nullptr;
	hr = device->CreateTexture2D(&depthDesc, nullptr, &temporaryDepth);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s %s", "Failed to create depthtexture");
		return false;
	}


	CRendertarget* newTarget = new CRendertarget();
	newTarget->myResource = resource;
	newTarget->myTarget = target;

	newTarget->mySize = GetTextureSize(resource, false);

	temporaryTexture->Release();
	temporaryDepth->Release();
	return newTarget;
} 

DXGI_FORMAT CTextureManager::GetTextureFormat(ID3D11ShaderResourceView* aResourceView) const
{
	ID3D11Resource* resource = nullptr;
	aResourceView->GetResource(&resource);
	if (!resource)
	{
		return DXGI_FORMAT_UNKNOWN;
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc(&desc);
	resource->Release();
	return desc.Format;
}

VECTOR2F CTextureManager::GetTextureSize( ID3D11ShaderResourceView* aResourceView, bool aNormalize) const
{
	ID3D11Resource* resource = nullptr;
	aResourceView->GetResource(&resource);
	if (!resource)
	{
		return VECTOR2F(0, 0);
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>( resource );
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc( &desc );

	VECTOR2F size(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	resource->Release();

	VECTOR2F windowSize;
	windowSize.x = static_cast<float>(CEngine::GetInstance()->GetTargetSize().y);
	windowSize.y = static_cast<float>(CEngine::GetInstance()->GetTargetSize().y);

	if (aNormalize)
	{
		return size / windowSize;
	}
	return size;
}

CTexture* CTextureManager::CreateTextureFromTarga(Tga32::Image* aImage)
{
	if (!aImage)
	{
		return nullptr;
	}
	ID3D11Texture2D* texture = nullptr;
	CTexture *tex = nullptr;
	ID3D11ShaderResourceView* resource = nullptr;

	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = aImage->myHeight;
	int w = aImage->myWidth;
	int *buf = new int[h*w];
	for (int i = 0; i < (h*w) * 4; i+=4)
	{
		unsigned char r = static_cast<unsigned char>(aImage->myImage[i]);
		unsigned char g = static_cast<unsigned char>(aImage->myImage[i+1]);
		unsigned char b = static_cast<unsigned char>(aImage->myImage[i+2]);
		unsigned char a = static_cast<unsigned char>(aImage->myImage[i+3]);

		unsigned int final = 0;
		final |= (a << 24);
		final |= (r << 16);
		final |= (g << 8);
		final |= (b);

		buf[i/4] = final;
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w * h * 4;

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DYNAMIC;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tdesc.MiscFlags = 0;

	if (FAILED(myDirect3D->GetDevice()->CreateTexture2D(&tdesc, &tbsd, &texture)))
		return nullptr;

	delete[] buf;

	HRESULT hr = myDirect3D->GetDevice()->CreateShaderResourceView(texture, NULL, &resource);
	if (FAILED(hr))
	{
		return nullptr;
	}

	if(resource)
	{
		tex = new CTexture();
		tex->myImageSize.Set(aImage->myWidth, aImage->myHeight);
		tex->myPath = "Lol";
		tex->myResource = resource;
	}

	return tex;
}

void CTextureManager::GeneratePerlinNoiseTexture(unsigned short aWidth, unsigned short aHeight)
{
	aWidth;
	aHeight;
#ifdef USE_NOISE
	if (!myNoiseTexture)
	{
		D3D11_TEXTURE2D_DESC tdesc;
		D3D11_SUBRESOURCE_DATA tbsd;

		int h = aHeight;
		int w = aWidth;
		int *buf = new int[h*w];
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j+=4)
			{
				int calc = i*w + j;
				buf[calc] = 0xffff00ff;
				buf[calc + 1] = 0xffff00ff;
				buf[calc + 2] = 0xffff00ff;
				buf[calc + 3] = 0xffff00ff;
			}
		}

		tbsd.pSysMem = (void *)buf;
		tbsd.SysMemPitch = w * 4;
		tbsd.SysMemSlicePitch = w*h * 4;

		tdesc.Width = w;
		tdesc.Height = h;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;

		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DYNAMIC;
		tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tdesc.MiscFlags = 0;

		if (FAILED(myDirect3D->GetDevice()->CreateTexture2D(&tdesc, &tbsd, &myNoiseTexture)))
			return;

		delete[] buf;

		HRESULT hr = myDirect3D->GetDevice()->CreateShaderResourceView(myNoiseTexture, NULL, &myNoiseResource);
		if (FAILED(hr))
		{
			return;
		}
	}

	CDirectEngine& engine = CEngine::GetInstance()->GetDirect3D();
	D3D11_MAPPED_SUBRESOURCE  mappedResource;
	HRESULT result = engine.GetContext()->Map(myNoiseTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Create a PerlinNoise object with the reference permutation vector


	PerlinNoise pn(rand() % 100);
	int* source = (int*)(mappedResource.pData);

	int h = aHeight;
	int w = aWidth;
	int *buf = new int[h*w];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			double x = (double)j / ((double)aWidth);
			double y = (double)i / ((double)aHeight);

			float n = static_cast<float>(pn.noise(200 * x, 240 * y, 0.8));

			CColor col(n, n, n, 1);

			int calc = i*w + j;
			source[calc] = col.AsHex();
		}
	}

	delete[] buf;

	engine.GetContext()->Unmap(myNoiseTexture, 0);

#endif

}

void CTextureManager::CreateErrorSquareTexture()
{
	HRESULT hr;

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 16;
	int w = 16;
	int *buf = new int[h*w];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if ((i+j) % 2 == 0)
			{
				buf[i*w+j] = 0xff000000;
			}
			else
			{
				buf[i*w+j] = 0xffff00ff;
			}
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w*4;
	tbsd.SysMemSlicePitch = w*h*4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if(FAILED( myDirect3D->GetDevice()->CreateTexture2D(&tdesc,&tbsd,&tex)))
		return;

	delete[] buf;

	hr = myDirect3D->GetDevice()->CreateShaderResourceView(tex, NULL, &myFailedResource);
	if (FAILED(hr))
	{
		return;
	}
	tex->Release();
}

ID3D11ShaderResourceView* CTextureManager::CreateWhiteSquareTexture()
{
	HRESULT hr;

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 4;
	int w = 4;
	int *buf = new int[h*w];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			buf[i*w + j] = 0xffffffff;
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w*h * 4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if (FAILED(myDirect3D->GetDevice()->CreateTexture2D(&tdesc, &tbsd, &tex)))
		return nullptr;

	delete[] buf;

	ID3D11ShaderResourceView* resource = nullptr;
	hr = myDirect3D->GetDevice()->CreateShaderResourceView(tex, NULL, &resource);
	if (FAILED(hr))
	{
		return nullptr;
	}
	tex->Release();
	return resource;
}

void CTextureManager::CreateDefaultNormalmapTexture()
{
	HRESULT hr;

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC tdesc;
	D3D11_SUBRESOURCE_DATA tbsd;

	int h = 4;
	int w = 4;
	int *buf = new int[h*w];
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			buf[i*w + j] = 0xffff8080;
		}
	}

	tbsd.pSysMem = (void *)buf;
	tbsd.SysMemPitch = w * 4;
	tbsd.SysMemSlicePitch = w*h * 4; // Not needed since this is a 2d texture

	tdesc.Width = w;
	tdesc.Height = h;
	tdesc.MipLevels = 1;
	tdesc.ArraySize = 1;

	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	tdesc.CPUAccessFlags = 0;
	tdesc.MiscFlags = 0;

	if (FAILED(myDirect3D->GetDevice()->CreateTexture2D(&tdesc, &tbsd, &tex)))
		return;

	delete[] buf;

	hr = myDirect3D->GetDevice()->CreateShaderResourceView(tex, NULL, &myDefaultNormalMapResource);
	if (FAILED(hr))
	{
		return;
	}
	tex->Release();
}

void Tga2D::CTextureManager::Init()
{
	
	myDirect3D = &CEngine::GetInstance()->GetDirect3D();
	CreateErrorSquareTexture();
	
	CTexture* newTexture = new CTexture();
	newTexture->myPath = "WhiteSquare";
	newTexture->myResource = CreateWhiteSquareTexture();
	
	newTexture->mySize = VECTOR2F(0.3f, 0.3f);
	newTexture->myImageSize = VECTOR2F(512, 512);
	myWhiteSquareTexture = newTexture;

	GeneratePerlinNoiseTexture(64, 64);

	CreateDefaultNormalmapTexture();
}

void Tga2D::CTextureManager::ReleaseTexture(CTexture* aTexture)
{
	aTexture->myResource->Release();
	aTexture->myResource = myFailedResource;
}

void Tga2D::CTextureManager::Update()
{
}
