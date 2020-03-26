#include "stdafx.h"
#include <cstdio>
#include <cassert>
#include <memory>
#include <iostream>
#include <windows.h>
#include <tga2d/tgaloader.h>

Tga32::Tga32()
{
}
Tga32::~Tga32()
{
}
Tga32::Image::Image()
{
}
Tga32::Image::~Image()
{
	delete myImage;
}

Tga32::Image *Tga32::Load( const char* aName )
{
	Image *myImage;
	TgaHeader header;
	
	myImage = new Image(); //allocates return object

	HANDLE handle = CreateFileA(aName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	HANDLE mapHandle = CreateFileMappingA(handle, NULL, PAGE_READONLY, NULL, NULL, nullptr);
	LPVOID ptr = MapViewOfFile(mapHandle, FILE_MAP_READ, NULL, NULL, NULL);
	header = *((TgaHeader*)ptr);

	//fopen_s(&fp, aName, "rb" );
	//assert( fp != NULL );
	
	//fread(&header,sizeof(header),1,fp); // loads the header
	
	myImage->myWidth	= header.myWidth;
	myImage->myHeight	= header.myHeight;

	if( header.myBpp == 32 )
	{
		const unsigned int imageSize = (myImage->myHeight*myImage->myWidth) << 2;
		myImage->myImage	= new unsigned char [imageSize];

		memcpy_s(myImage->myImage, imageSize, (unsigned char*)ptr + sizeof(header), imageSize);

		//UnmapViewOfFile(ptr);

		//fread( myImage->myImage, myImage->myHeight * myImage->myWidth * 4, 1, fp ); // loads the image

		//fclose( fp );

		FlipImageDataFast(header, myImage->myImage);

		myImage->myBitDepth=32;
		return(myImage);
	}
	
	return( NULL );
}

#define xorSwap(x, y) (x) = std::exchange((y), (x));

void Tga32::FlipImageDataFast(const TgaHeader & header, unsigned char * source)
{
	const unsigned int height = header.myHeight;
	const int width = header.myWidth;
	const unsigned int width4 = width << 2;
	const unsigned int width8 = width << 3;
	const int height2 = height >> 1;

	int x = -1, y = -1;

	int row1 = -4;
	unsigned int row2 = height * width4 - width4 - 4;

	unsigned int row12;
	unsigned int row13;
	unsigned int row14;
	unsigned int row21;
	unsigned int row22;
	unsigned int row24;

	while(++y<height2)
	{
		x = -1;
		while(++x<width)
		{
			row1 += 4;
			row12 = row1 + 1;
			row13 = row1 + 2;
			row14 = row1 + 3;

			row2 += 4;
			row21 = row2 + 2;
			row22 = row2 + 1;
			row24 = row2 + 3;

			xorSwap(
				source[row1],
				source[row21]
			)
			xorSwap(
				source[row12],
				source[row22]
			)
			xorSwap(
				source[row13],
				source[row2]
			)
			xorSwap(
				source[row14],
				source[row24]
			)
		}
		row2 -= width8;
	}
}


