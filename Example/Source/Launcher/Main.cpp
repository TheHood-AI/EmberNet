#include "../../../EmberNet/CommonNetworkIncludes.h"
#include "../Game/Game.h"
#include "VersionNo.h"
#include <string>
#include <stringapiset.h>
#include <atlstr.h>

void Init(int aX, int aY);

#ifdef _DEBUG
#pragma comment(lib,"Game_Debug.lib")
#endif // DEBUG
#ifdef _RELEASE
#pragma comment(lib,"Game_Release.lib")
#endif // _RELEASE
#ifdef _RETAIL
#pragma comment(lib,"Game_Retail.lib")
#endif // _RETAIL


// Comment out below define to disable command line

#ifndef _RETAIL
#define USE_CONSOLE_COMMAND
#endif

void StartExe(LPCTSTR aExePath, int aInstanceIndex)
{
	aInstanceIndex += 1;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	int x = ((aInstanceIndex) % 2);
	int y = ((aInstanceIndex) / 2);

	std::wstring cmdWindowX = L"WINPOSX:" + std::to_wstring(x * 1920 / 2);
	std::wstring cmdWindowY = cmdWindowX + L"WINPOSY:" + std::to_wstring(std::clamp(y * (1080 / 2) - 10, 0, 1920));

	CreateProcess(aExePath,
		&cmdWindowY[0],
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

#pragma region WindowsInit
void InitConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
#pragma warning( pop )
}

void CloseConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
#pragma warning( pop )
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, char*, int /*nShowCmd*/)
{
	Sleep(1000);

#ifdef USE_CONSOLE_COMMAND
	InitConsole();
#endif

	constexpr char nrOfClients = 1;
	if (IsDebuggerPresent())
	{
#ifdef _DEBUG
		for (char i = 0; i < nrOfClients - 1; ++i)
		{
			StartExe(L"Launcher_Debug.exe", i);
		}
#else
		for (char i = 0; i < nrOfClients - 1; ++i)
		{
			StartExe(L"Launcher_Release.exe", i);
		}
#endif
	}

	LPWSTR* szArgList;
	int argCount;
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	int myWindowPosX = 0;
	int myWindowPosY = 0;

	for (int i = 0; i < argCount; ++i)
	{
		std::string simpleString = CW2A(szArgList[i]);

		if (simpleString.find("WINPOSX") != std::string::npos)
		{
			simpleString.erase(0, 8);
			std::cout << simpleString << std::endl;

			myWindowPosX = std::stoi(simpleString, nullptr, 0);
			std::cout << "X: " << myWindowPosX << std::endl;

		}
		if (simpleString.find("WINPOSY") != std::string::npos)
		{
			simpleString.erase(0, simpleString.find("WINPOSY") + 8);
			std::cout << simpleString << std::endl;

			myWindowPosY = std::stoi(simpleString, nullptr, 0);
			std::cout << "Y: " << myWindowPosY << std::endl;

		}
	}

	Init(myWindowPosX, myWindowPosY);

#ifdef USE_CONSOLE_COMMAND
	CloseConsole();
#endif
	return 0;
	}
#pragma endregion MainFunction


void Init(int aX, int aY)
{
	std::cout << sizeof(float) << std::endl;
	CGame myGame;

	int version[] = { PRODUCTVER };
	std::wstring versionNumber;

	int count = sizeof(version) / sizeof(version[0]);
	for (int i = 0; i < count; i++)
	{
		versionNumber += std::to_wstring(version[i]);
		versionNumber += L",";
	}
	versionNumber.pop_back();

	myGame.Init(versionNumber, nullptr, aX, aY); // Blocking
}


