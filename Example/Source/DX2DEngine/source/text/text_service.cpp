#include "stdafx.h"

#include <ft2build.h>

#ifdef _DEBUG
#pragma comment (lib, "freetype_Debug_x64.lib")
#else
#pragma comment (lib, "freetype_Release_x64.lib")
#endif

#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include FT_OUTLINE_H

#include "text/text_service.h"
#include "texture/texture_manager.h"
#include "text/textfile.h"
#include "text/fontfile.h"
#include "sprite/sprite_batch.h"
#include "sprite/sprite.h"
#include "text/text.h"
#include "sprite/textured_quad_batch.h"
#include "engine.h"
#include <fstream>
#include <tga2d/shaders/customshader.h>



#define X_OFFSET 8
#define X_START 2
#define Y_OFFSET 8

using namespace Tga2D;

CFontData::~CFontData()
{
	SAFE_DELETE(myAtlas);
	if (myBatch)
	{
		if (myBatch->myQuadBatch)
		{
			SAFE_DELETE(myBatch->myQuadBatch->myTexture);
		}
	}
	
	SAFE_DELETE(myBatch);
}

CTextService::CTextService()
{

	myCurrentSprite = 0;
	myTextsToDraw = 0;

}


CTextService::~CTextService()
{
	delete[]mySprites;

	for (std::unordered_map<std::string, CFontData*>::iterator iterator = myFontData.begin(); iterator != myFontData.end(); iterator++)
	{
		delete iterator->second;
	}
	myFontData.clear();

}

void CTextService::Init()
{
	mySprites = new Tga2D::CSprite[MAX_TEXT_SPRITES]();
	FT_Init_FreeType(&myLibrary);
}

inline bool FileExists(const std::string& name) 
{
	std::ifstream f(name.c_str());
	if (f.good()) 
	{
		f.close();
		return true;
	}

	f.close();
	return false;
}

CFontData* CTextService::InitAndLoad(std::string aFontPath, unsigned char aBorderSize, int aSize)
{
	if (aSize == -1)
	{
		return nullptr;
	}
	
	short fontWidth = (short)aSize;
	const int atlasSize = 1024;
	float atlasWidth = static_cast<float>(atlasSize);
	float atlasHeight = static_cast<float>(atlasSize);

	int atlasX = X_START;
	int atlasY = 2;
	int currentMaxY = 0;

	int aBorderWidth = aBorderSize;

	CFontData* fontData = nullptr;


	std::stringstream key;
	key << aFontPath << "-" << fontWidth;


	if (myFontData.find(key.str()) == myFontData.end())
	{
		fontData = new CFontData;
		myFontData[key.str()] = fontData;

	}
	else
	{
		myFontData[key.str()]->myRefCount++;
		return myFontData[key.str()];
	}

	fontData->myAtlas = new int[atlasSize * atlasSize];
	ZeroMemory(fontData->myAtlas, (atlasSize * atlasSize) * sizeof(int));
	fontData->myFontHeightWidth = fontWidth;
	fontData->myName = key.str();

	FT_Face face;
	FT_Error error = FT_New_Face(myLibrary, aFontPath.c_str(), 0, &face);
	if (error != 0)
	{
		return nullptr;
	}

	FT_F26Dot6 ftSize = (FT_F26Dot6)(fontData->myFontHeightWidth * (1 << 6));
	error = FT_Set_Char_Size(face, ftSize, 0, 100, 100);
	if (error != 0)
	{
		ERROR_PRINT("%s", "FT_Set_Char_Size error");
		return nullptr;
	}

	error = FT_Load_Char(face, 'x', FT_LOAD_DEFAULT);
	if (error != 0)
	{
		ERROR_PRINT("%s", "Failed to load glyph!");
		return nullptr;
	}


	FT_GlyphSlot space = face->glyph;

	fontData->myWordSpacing = static_cast<short>(space->metrics.width / 256);

	int currentMax = 256;
	int currentI = 32;

	for (int i = currentI; i < currentMax; i++)
	{
		error = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (error != 0)
		{
			ERROR_PRINT("%s%s", "Failed to load glyph! ", (char)i);
			continue;
		}

		FT_GlyphSlot slot = face->glyph;

		if (atlasX + slot->bitmap.width + (aBorderWidth * 2) > atlasWidth)
		{
			atlasX = X_START;
			atlasY = currentMaxY;
		}
		if (aBorderWidth > 0)
		{
			LoadOutline(i, atlasX, atlasY, atlasWidth, fontData, face, aBorderWidth);
		}

		LoadGlyph(i, atlasX, atlasY, currentMaxY, atlasWidth, atlasHeight, fontData, face, aBorderWidth);
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = fontData->myAtlas;
	data.SysMemPitch = atlasSize * 4;

	D3D11_TEXTURE2D_DESC info;
	info.Width = atlasSize;
	info.Height = atlasSize;
	info.MipLevels = 0;
	info.ArraySize = 1;
	info.SampleDesc.Count = 1;
	info.SampleDesc.Quality = 0;
	info.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.Usage = D3D11_USAGE_DEFAULT;
	info.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	info.CPUAccessFlags = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = CEngine::GetInstance()->GetDirect3D().GetDevice()->CreateTexture2D(&info, nullptr, &texture);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s", "Failed to load texture for text!");
		return nullptr;
	}

	CEngine::GetInstance()->GetDirect3D().GetContext()->UpdateSubresource(texture, 0, NULL, fontData->myAtlas, atlasSize * 4, 0);

	CEngine::GetInstance()->GetDirect3D().GetDevice()->CreateShaderResourceView(texture, nullptr, &fontData->myAtlasView);
	CEngine::GetInstance()->GetDirect3D().GetContext()->GenerateMips(fontData->myAtlasView);
	texture->Release();

	fontData->myAtlasHeight = atlasSize;
	fontData->myAtlasWidth = atlasSize;
	fontData->myLineSpacing = static_cast<float>((face->ascender - face->descender) >> 6);
	FT_Done_Face(face);
	
	fontData->myBatch = new CSpriteBatch(false);
	fontData->myBatch->Init(nullptr);

	fontData->myBatch->myQuadBatch->myIsScreenSpace = true;

	fontData->myBatch->myQuadBatch->myTexture = new CTexture();
	fontData->myBatch->myQuadBatch->myTexture->myResource = fontData->myAtlasView;
	fontData->myBatch->myQuadBatch->myTexture->myPath = key.str();
	
	return fontData;

}

void CTextService::Update()
{
	myCurrentSprite = 0;
}


void Tga2D::CTextService::RemoveFontRef(CFontData *aData)
{
	for (std::unordered_map<std::string, CFontData*>::iterator iterator = myFontData.begin(); iterator != myFontData.end(); iterator++)
	{
		if (iterator->second== aData)
		{
			iterator->second->myRefCount--;
			if (iterator->second->myRefCount <= 0)
			{
				delete iterator->second;
				myFontData.erase(iterator);
			}
			break;
		}
	}
}

float Tga2D::CTextService::GetSentenceWidth(Tga2D::CText* aText)
{
	if (!aText)
	{
		return 0.0f;
	}
	if (aText->myText.size() <= 0)
	{
		return 0.0f;
	}

	CFontData* fontData = aText->myFontDataPointer;
	if (!fontData)
	{
		return 0.0f;
	}

	float drawX = 0.f;

	int count = static_cast<int>(aText->myText.length());

	// Compute middle
	float maxX = 0;
	float wordSpace = 0;
	for (int i = 0; i < count; i++)
	{
		const char theChar = aText->myText[i];
		int charAsNumber = (unsigned char)theChar;
		SCharData& charData = fontData->myCharData[charAsNumber];


		if (theChar == '\n')
		{
			maxX = 0;
			continue;
		}

		maxX += charData.myAdvanceX;
		if (maxX > drawX)
		{
			drawX = maxX;
		}

		if (theChar == ' ')
		{
			wordSpace += fontData->myWordSpacing;
		}
	}

	return (drawX+ wordSpace) * aText->myScale;
}


float Tga2D::CTextService::GetSentenceHeight(Tga2D::CText* aText)
{
	if (!aText)
	{
		return 0.0f;
	}
	if (aText->myText.size() <= 0)
	{
		return 0.0f;
	}

	CFontData* fontData = aText->myFontDataPointer;
	if (!fontData)
	{
		return 0.0f;
	}

	float drawX = 0.f;

	int count = static_cast<int>(aText->myText.length());

	// Compute middle
	float maxY = 0;
	float maxDrawY = 0;
	for (int i = 0; i < count; i++)
	{
		const char theChar = aText->myText[i];
		int charAsNumber = (unsigned char)theChar;
		SCharData& charData = fontData->myCharData[charAsNumber];

		if (maxDrawY < charData.myHeight)
		{
			maxDrawY = static_cast<float>(charData.myHeight);
		}

		if (theChar == '\n')
		{
			drawX += (maxDrawY + 6);
			continue;
		}

		int sizeY = charData.myHeight;
		if (sizeY > maxY)
		{
			maxY = static_cast<float>(sizeY);
		}
	}

	return (drawX + maxY) * aText->myScale;
}

void Tga2D::CTextService::ClearMemory(const std::string& aRenderName)
{
	CFontData* fontData = myFontData[aRenderName];
	if (!fontData)
	{
		return;
	}

	delete myFontData[aRenderName];
	myFontData.erase(aRenderName);
}

bool CTextService::UpdateTextAndDraw(const std::string& aText, const VECTOR2F& aPosition, CColor aColor, float aSize, CFontData* aDataPtr, float aRotation, Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith)
{
	CFontData* fontData = aDataPtr;
	if (!fontData || !fontData->myBatch)
	{
		return false;
	}

	float screenSizeX = static_cast<float>(CEngine::GetInstance()->GetTargetSize().x);
	float screenSizeY = static_cast<float>(CEngine::GetInstance()->GetTargetSize().y);

	fontData->myBufferedObjects.ResetCount();
	if (aText.size() > fontData->myBufferedObjects.TotalCount())
	{
		//We have everything we need, but with a too large text amount
		ERROR_PRINT("%s%i%s%i","CTextService: Trying to draw a text that is to large! Reduce the text or increase the allowed amount in engine_defines.h. Currently supporting: ", FONT_BUFF_SIZE, " Your text size: ", aText.size())
		return true;
	}
	BuildText(aText, aColor, aSize, aDataPtr, fontData->myBufferedObjects);

	float midX = 0;
	float midY = 0;
	if (aRotation != 0.0f)
	{
		for (int i = 0; i < fontData->myBufferedObjects.NextCount() + 1; i++)
		{
			STextToRender& text = fontData->myBufferedObjects.myData[i];
			midX += text.myPosition.x;
			midY += text.myPosition.y;
		}

		midX /= screenSizeX;
		midY /= screenSizeY;
		midX /= fontData->myBufferedObjects.NextCount();
		midY /= fontData->myBufferedObjects.NextCount();
	}

	float windowRatio = static_cast<float>(CEngine::GetInstance()->GetWindowRatio());

	//if (fontData->myRenderCountChanged)
	{
		fontData->myRenderCountChanged = false;
		fontData->myBatch->ClearAll();
	}
	for (int i = 0; i < fontData->myBufferedObjects.NextCount()+1; i++)
	{
		if (i > FONT_BUFF_SIZE)
		{
			break;
		}
		STextToRender& text = fontData->myBufferedObjects.myData[i];

	
		if (myCurrentSprite < MAX_TEXT_SPRITES)
		{
			Tga2D::CSprite* sprite = &mySprites[myCurrentSprite];

			text.myPosition.x /= screenSizeX;
			text.myPosition.y /= screenSizeY;

			text.mySize.x /= screenSizeY;
			text.mySize.y /= screenSizeY;

			VECTOR2F newPos = text.myPosition;

			if (aRotation != 0.0f)
			{
				sprite->SetRotation(aRotation);

				float diffX = midX;
				float diffY = midY;

				float c = cos(aRotation);
				float s = sin(aRotation);

				newPos.x -= diffX;
				newPos.y -= diffY;

				newPos.y /= windowRatio;

				float xnew = newPos.x * c - newPos.y * s;
				float ynew = (newPos.x * s + newPos.y * c) * windowRatio;

				newPos.x = xnew + diffX;
				newPos.y = ynew + diffY;
			}
			

			sprite->ClearBatchData();
			sprite->SetPosition(newPos + aPosition );
			sprite->SetSizeRelativeToScreen(text.mySize * aSize);
			sprite->SetColor(text.myColor);

			sprite->SetTextureRect(text.myUV.x, text.myUV.y, text.uvScale.x, text.uvScale.y);
			fontData->myBatch->AddObject(sprite);
			myCurrentSprite++; 
		}
	}
	fontData->myBatch->SetSamplerState(ESamplerFilter_Bilinear, ESamplerAddressMode_Clamp);
	if (fontData->myFontHeightWidth < 18)
	{
		fontData->myBatch->SetSamplerState(ESamplerFilter_Point, ESamplerAddressMode_Clamp);
	}
	
	fontData->myBatch->SetCustomShader(aCustomShaderToRenderWith);
	fontData->myBatch->Render();
	 
	return true;
}

void CTextService::BuildText(const std::string& aText, CColor aColor, float aSize, CFontData* aDataPtr, SSimplePointerArray<STextToRender>& aTextBuffer)
{
	CFontData* fontData = aDataPtr;
	if (!fontData)
	{
		return;
	}

	int count = static_cast<int>(aText.length());
	if (aDataPtr->myLastRenderCount != count)
	{
		aDataPtr->myLastRenderCount = count;
		aDataPtr->myRenderCountChanged = true;
	}
	float drawX = 0.f;
	float drawY = 0.f;
	float maxDrawY = 0.f;
	 
	for (int i = 0, row = 0; i < count; i++)
	{
		const char theChar = aText[i];
		int charAsNumber = (unsigned char)theChar;
		SCharData& charData = fontData->myCharData[charAsNumber];

		if (maxDrawY < charData.myHeight)
		{
			maxDrawY = static_cast<float>(charData.myHeight);
		}


		if (theChar == '\n')
		{
			drawX = 0;
			drawY += (maxDrawY + 6);
			row++;
			continue;
		}

		if (theChar == ' ')
		{
			drawX += fontData->myWordSpacing;
		}

		STextToRender* renderText = aTextBuffer.GetNext();
		if (!renderText)
		{
			// Buffer is depleted!
			break;
		}

		renderText->myPosition.Set((drawX + charData.myBearingX) * aSize, (drawY + charData.myBearingY) * aSize);
		renderText->mySize.Set(static_cast<float>(charData.myWidth), static_cast<float>(charData.myHeight));

		renderText->myUV = charData.myTopLeftUV;
		renderText->uvScale = charData.myBottomRightUV;
		renderText->myColor = aColor;

		drawX += charData.myAdvanceX;
	}
}

bool CTextService::AddTextToRender(const std::string& aText, const VECTOR2F& aPosition, CColor aColor, float aSize, CFontData* aDataPtr, float aRotation, Tga2D::CCustomShaderBatched* aCustomShaderToRenderWith)
{
	return UpdateTextAndDraw(aText, aPosition, aColor, aSize, aDataPtr, aRotation, aCustomShaderToRenderWith);
}

void CTextService::CountOffsets(const int& x, const int& y, const int& width, const int& height, SCountData& countData)
{
	if (x < 1)
	{
		countData.xNCount++;
	}

	if (x > width - 1)
	{
		countData.xPCount++;
	}

	if (y < 1)
	{
		countData.yNCount++;
	}

	if (y > height - 1)
	{
		countData.yPCount++;
	}
}

void CTextService::CountDeltas(const int& width, const int& height, int& deltaX, int& deltaY, SCountData& countData)
{
	if (countData.xNCount == height)
	{
		countData.xNCount = 0;
		deltaX--;
	}

	if (countData.xPCount == height)
	{
		countData.xPCount = 0;
		deltaX++;
	}

	if (countData.yNCount == width)
	{
		countData.yNCount = 0;
		deltaY--;
	}

	if (countData.yPCount == width)
	{
		countData.yPCount = 0;
		deltaY++;
	}
}

void CTextService::CalculateOutlineOffsets(const int index, FT_FaceRec_* aFace, int aBorderOffset)
{
	myTextOutlineOffset.xDelta = 0;
	myTextOutlineOffset.yDelta = 0;

	FT_Error err;
	FT_Stroker stroker;
	FT_Glyph glyph;

	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to load glyph!");
	}
	err = FT_Get_Glyph(aFace->glyph, &glyph);

	glyph->format = FT_GLYPH_FORMAT_OUTLINE;

	err = FT_Stroker_New(myLibrary, &stroker);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to get glyph!");
	}
	

	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to stroke glyph border");
	}


	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to add glyph to bitmap");
	}
	
	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

	unsigned int width = bitmapGlyph->bitmap.width;
	unsigned int height = bitmapGlyph->bitmap.rows;

	int xDelta = 0;
	int yDelta = 0;

	SCountData countData;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned char& data = bitmapGlyph->bitmap.buffer[y * width + x];

			if (data == 0)
			{
				CountOffsets(x, y, width, height, countData);
			}
			CountDeltas(width, height, xDelta, yDelta, countData);
		}
	}

	myTextOutlineOffset.xDelta = xDelta;
	myTextOutlineOffset.yDelta = yDelta;

	FT_Stroker_Done(stroker);
}

void CTextService::CalculateGlyphOffsets(const int /*index*/, FT_GlyphSlotRec_* glyph)
{
	int xDelta = 0;
	int yDelta = 0;

	unsigned int width = glyph->bitmap.width;
	unsigned int height = glyph->bitmap.rows;

	SCountData countData;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned char& data = glyph->bitmap.buffer[y * width + x];

			if (data == 0)
			{
				CountOffsets(x, y, width, height, countData);
			}
			CountDeltas(width, height, xDelta, yDelta, countData);
		}
	}

	myTextOutlineOffset.xDelta = xDelta;
	myTextOutlineOffset.yDelta = yDelta;
}

int Color32Reverse2(int x)
{
	return ((x & 0xFF000000) >> 8) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
}

int Color32Reverse(int x)
{
	int returner = x;
	returner |= ((x & 0x000000FF) << 24);
	returner |= ((x & 0x000000FF) << 16);
	returner |= ((x & 0x000000FF) << 8);

	return returner;
}


void CTextService::LoadGlyph(int index, int& atlasX, int& atlasY, int& maxY, float atlasWidth, float atlasHeight, CFontData* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
{
	FT_Error error = FT_Load_Char(aFace, index, FT_LOAD_RENDER);
	if (error != 0)
	{
		ERROR_PRINT("%s", "Failed to load char while glyph loading");
		return;
	}

	FT_GlyphSlot slot = aFace->glyph;
	FT_Bitmap bitmap = slot->bitmap;

	int height = bitmap.rows;
	int width = bitmap.width;

	SCharData glyphData;
	glyphData.myChar = static_cast<char>(index); 
	glyphData.myHeight = static_cast<short>(height + (aBorderOffset * 2));
	glyphData.myWidth = static_cast<short>(width + (aBorderOffset * 2));

	glyphData.myTopLeftUV = { (float(atlasX) / atlasWidth), (float(atlasY) / atlasHeight) };
	glyphData.myBottomRightUV = { (float(atlasX + glyphData.myWidth) / atlasWidth), (float(atlasY + glyphData.myHeight) / atlasHeight) };
	 
	glyphData.myAdvanceX = static_cast<short>(slot->advance.x) >> 6;
	glyphData.myAdvanceY = static_cast<short>(slot->advance.y) >> 6;

	glyphData.myBearingX = (short)slot->bitmap_left;
	glyphData.myBearingY = (short)-slot->bitmap_top;
	if (glyphData.myTopLeftUV.x > 1 || glyphData.myTopLeftUV.y > 1 || glyphData.myBottomRightUV.x > 1 || glyphData.myBottomRightUV.y > 1)
	{
		ERROR_PRINT("%s", "Tried to set a glyph UV to above 1");
		return;
	} 

	CalculateGlyphOffsets(index, slot);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (x < 0 || y < 0)
			{
				continue;
			}
			int& saved = aFontData->myAtlas[((atlasY)+aBorderOffset + myTextOutlineOffset.yDelta + y) *
				int(atlasWidth) + ((atlasX + aBorderOffset + myTextOutlineOffset.xDelta) + x)];
			saved |= bitmap.buffer[y * bitmap.width + x];

			saved = Color32Reverse(saved);

			if (y + (atlasY + Y_OFFSET) > maxY)
			{
				maxY = y + (atlasY + Y_OFFSET);
			}
		}
	}

	atlasX = atlasX + width + X_OFFSET;
	aFontData->myCharData[index] = glyphData;
}

void CTextService::LoadOutline(const int index, const int atlasX, const int atlasY , const float atlasWidth, CFontData* aFontData, FT_FaceRec_* aFace, int aBorderOffset)
{

	FT_Error err;
	FT_Stroker stroker;
	FT_Glyph glyph;

	err = FT_Load_Char(aFace, index, FT_LOAD_NO_BITMAP);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to load glyph!");
	}

	err = FT_Get_Glyph(aFace->glyph, &glyph);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to get glyph!");
	}

	glyph->format = FT_GLYPH_FORMAT_OUTLINE;

	err = FT_Stroker_New(myLibrary, &stroker);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to get stroker!");
	}


	FT_Stroker_Set(stroker, aBorderOffset * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
	err = FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to stroke!");
	}


	err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
	if (err != 0)
	{
		ERROR_PRINT("%s", "Failed to add glyph to bitmap");
	}
	
	FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

	unsigned int width = bitmapGlyph->bitmap.width;
	unsigned int height = bitmapGlyph->bitmap.rows;

	CalculateOutlineOffsets(index, aFace, aBorderOffset);
	for (unsigned  int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{

			int& data = aFontData->myAtlas[((atlasY + myTextOutlineOffset.yDelta) + y) * int(atlasWidth) + ((atlasX + myTextOutlineOffset.xDelta) + x)];
			data = 0;
			data |= bitmapGlyph->bitmap.buffer[y * width + x];
			data = Color32Reverse2(data);

		}
	}

	FT_Stroker_Done(stroker);
}