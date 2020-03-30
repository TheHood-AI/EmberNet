#ifndef _OPTIMIZABLE_TGA_H_
#define _OPTIMIZABLE_TGA_H_

class Tga32
{
public:		struct Image
			{
				unsigned short myWidth;
				unsigned short myHeight;
				unsigned char myBitDepth;
				unsigned char *myImage;
				Image();
				~Image();
			};
				
				
			struct TgaHeader {
				unsigned char myIdLength;
				unsigned char myColorMapType;
				unsigned char myImageType;
				unsigned char myColorMapOrigin;
				unsigned short myColorMapLength;
				unsigned char myColorMapEntrySize;
				unsigned short myImageOriginX;
				unsigned short myImageOriginY;
				unsigned short myWidth;
				unsigned short myHeight;
				unsigned char myBpp;
				unsigned char myImageDescriptor;
			};
				
				
			Tga32();
			~Tga32();
				
			static Image* Load( const char* aName );
			static void FlipImageDataFast( const TgaHeader& myHeader, unsigned char* source);
};



#endif // _OPTIMIZABLE_TGA_H_
