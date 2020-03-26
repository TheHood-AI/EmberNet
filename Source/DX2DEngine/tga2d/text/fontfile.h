#ifndef __FONTFILE_H__
#define __FONTFILE_H__

typedef unsigned int uint;
struct fontChar
{
	fontChar(){}
	fontChar(uint id, uint x, uint y, uint width, uint height, float xoffset, float yoffset, float xadvance, uint page, uint chnl, uint texWidth, uint texHeight) :
		id_(id), x_(x), y_(y), width_(width), height_(height), xoffset_(xoffset), yoffset_(yoffset), xadvance_(xadvance), page_(page), chnl_(chnl)
	{
		sMin_ = static_cast<float>(x)/static_cast<float>(texWidth);
		tMin_ = static_cast<float>(y)/static_cast<float>(texHeight);

		sMax_ = static_cast<float>(x+width)/static_cast<float>(texWidth);
		tMax_ = static_cast<float>(y+height)/static_cast<float>(texHeight);
	}

	uint id_;
	uint x_;
	uint y_;
	uint width_;
	uint height_;
	float xoffset_;
	float yoffset_;
	float xadvance_;
	uint page_;
	uint chnl_;

	float sMin_;
	float sMax_;
	float tMin_;
	float tMax_;
};

char *load_asset(const char* aTexturePath, int &aSize);
bool ParseFontFile( std::vector<fontChar> &fontChars, std::string &fontfilename, uint texwidth, uint texheight );
	

#endif //__FONTFILE_H__