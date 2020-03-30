#pragma once
#include <stringapiset.h>
#include <sstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4100) //Unreferenced formal parameter

namespace Tga2D
{
/*
	static wchar_t *CharArrayToLPCWSTR(const char* charArray)
	{
		wchar_t* wString=new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
		return wString;
	}
*/

	static std::string FloatToString(float aFloat)
	{
		std::ostringstream ss;
		ss << aFloat;
		std::string s(ss.str());
		return s;
	}

	static int StringToInt(const std::string& aString)
	{
		return atoi(aString.c_str());
	}

	static float StringToFloat(std::string aString)
	{
		std::string::size_type sz;
		float mars = std::stof (aString,&sz); 
		return mars;
	}

/*
	static std::wstring ConvertCharArrayToLPCWSTR(const char* charArray)
	{
		wchar_t* wString = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);

		std::wstring str(wString);
		delete[] wString;
		return str;
	}
*/

	static std::wstring ConvertStringToWstring(const std::string& aString)
	{
		std::wstring ws;
		ws.assign(aString.begin(), aString.end());
		return ws;
	}

	static std::string ConvertWStringToString(const std::wstring& aString)
	{
		std::string ws;
//		ws.assign(aString.begin(), aString.end());
		return ws;
	}



	template <typename T>
	static T clamp(const T& n, const T& lower, const T& upper) 
	{
		return std::fmax(lower, std::fmin(n, upper));
	}

}

#pragma warning(pop)
#pragma warning(disable: 4505) //Unreferenced local function has been removed