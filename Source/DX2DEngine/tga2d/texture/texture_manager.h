/*
This class handles all the textures created, please use this when you create a texture
*/

#pragma once
#include "Texture.h"
#include <dxgiformat.h>
#include <vector>
#include <tga2d/tgaloader.h>

struct ID3D11ShaderResourceView;
namespace Tga2D
{
	class CDirectEngine;
	class CSprite;
	class CTextureManager
	{
	public:
		CTextureManager(void);
		~CTextureManager(void);
		void Init();
		CTexture* GetTexture(const char* aTexturePath, bool aForceReload = false);
		CRendertarget* GetRendertarget(const VECTOR2UI& aSize);
		
		CTexture* GetWhiteSquareTexture() { return myWhiteSquareTexture; }
		VECTOR2F GetTextureSize(struct ID3D11ShaderResourceView* aResourceView, bool aNormalize = true) const;

		CTexture * CreateTextureFromTarga(Tga32::Image * aImage);

		void ReleaseTexture(CTexture* aTexture);

		void Update();

		/* Requires DX11 includes */
		struct ID3D11ShaderResourceView* GetDefaultNormalMapResource() const { return myDefaultNormalMapResource; }
		/* Requires DX11 includes */
		struct ID3D11ShaderResourceView* GetNoiseTexture() const { return myNoiseResource; }
	private:
		DXGI_FORMAT GetTextureFormat(struct ID3D11ShaderResourceView* aResourceView) const;
		std::vector<CTexture*> myResourceViews;
		void GeneratePerlinNoiseTexture(unsigned short aWidth, unsigned short aHeight);
		void CreateErrorSquareTexture();
		ID3D11ShaderResourceView* CreateWhiteSquareTexture();
		void CreateDefaultNormalmapTexture();
		void OnTextureChanged(std::string aFile);
		
		struct ID3D11Texture2D *myNoiseTexture;
		struct ID3D11ShaderResourceView* myNoiseResource;
		struct ID3D11ShaderResourceView* myFailedResource;
		/*ID3D11ShaderResourceView* myWhiteSquareResource;*/
		struct ID3D11ShaderResourceView* myDefaultNormalMapResource;
		class CDirectEngine* myDirect3D;
		class CTexture* myWhiteSquareTexture;
	};
}