#pragma once

namespace Tga2D
{
	namespace Tga2DText
	{
		struct SCharData
		{
			VECTOR2F myTopLeftUV;
			VECTOR2F myBottomRightUV;

			short myWidth;
			short myHeight;
			short myAdvanceX; //Distance to next character.
			short myAdvanceY;
			short myBearingX;
			short myBearingY;
			char myChar;
		};

		struct STextToRender
		{
			VECTOR2F mySize;
			VECTOR2F myUV;
			VECTOR2F uvScale;
			VECTOR2F myPosition;
			CColor myColor;
		};

		struct SCountData
		{
			int xNCount = 0;
			int xPCount = 0;
			int yNCount = 0;
			int yPCount = 0;
		};

		struct SOutlineOffset
		{
			int xDelta = 0;
			int yDelta = 0;

		};
	}
}