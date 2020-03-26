#pragma once
#include <tga2d/Engine.h>
#include <fstream>
#include "GameWorld.h"

#include "InputHandler.hpp"
class CGame
{
public:
	CGame();
	~CGame();
	bool Init(const std::wstring& aVersion = L"", HWND aHWND = nullptr, int aX = 0, int aY = 0);
	static InputHandler ourInputHandler;
	static bool ShouldQuit;
private:
	void InitCallBack();
	void UpdateCallBack();
	LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	C_GameWorld myGameWorld;

};
