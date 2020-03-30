#include "stdafx.h"
#include "InputHandler.hpp"

#include <Windows.h>
#include <windowsx.h>
InputHandler::InputHandler()
{

}

bool InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
		myScrollWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		
		break;

	case WM_MOUSEMOVE:
		myMousePos.x = static_cast<float>(GET_X_LPARAM(lParam));
		myMousePos.y = static_cast<float>(GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONDOWN:

		myNewStates[VK_LBUTTON] = true;
		break;

	case WM_LBUTTONUP:

		myNewStates[VK_LBUTTON] = false;
		break;

	case WM_MBUTTONDOWN:

		myNewStates[VK_MBUTTON] = true;
		break;

	case WM_MBUTTONUP:

		myNewStates[VK_MBUTTON] = false;
		break;

	case WM_RBUTTONDOWN:

		myNewStates[VK_RBUTTON] = true;
		break;

	case WM_RBUTTONUP:

		myNewStates[VK_RBUTTON] = false;
		break;

	case WM_KEYDOWN:
		myNewStates[wParam] = true;

		break;

	case WM_KEYUP:
		myNewStates[wParam] = false;
		break;

	default:

		return false;
		break;
	}
	
	return true;
}

void InputHandler::FinalizeUpdate()
{
	myOldStates = myNewStates;
	myOldMousePos = myMousePos;
	myScrollWheelDelta = 0;
}


bool InputHandler::IsKeyDown(char aKeyCode)
{
	return myNewStates[toupper(aKeyCode)];
}

bool InputHandler::IsKeyHit(char aKeyCode)
{
	return !myOldStates[toupper(aKeyCode)] && myNewStates[toupper(aKeyCode)];

}

bool InputHandler::IsKeyReleased(char aKeyCode)
{
	return myOldStates[toupper(aKeyCode)] && !myNewStates[toupper(aKeyCode)];
}

void InputHandler::SetMousePos(int someX, int someY)
{
	SetCursorPos(someX, someY);
}

bool InputHandler::IsMouseButtonDown(int aMouseButton)
{
	switch (aMouseButton)
	{
	case 0:
		return myNewStates[VK_LBUTTON];
	case 1:
		return myNewStates[VK_RBUTTON];
	case 2:
		return myNewStates[VK_MBUTTON];
	default:
		break;
	}
	return false;
}

bool InputHandler::IsMouseButtonHit(int aMouseButton)
{
	switch (aMouseButton)
	{
	case 0:
		return !myOldStates[VK_LBUTTON] && myNewStates[VK_LBUTTON];
	case 1:
		return !myOldStates[VK_RBUTTON] && myNewStates[VK_RBUTTON];
	case 2:
		return !myOldStates[VK_MBUTTON] && myNewStates[VK_MBUTTON];
	default:
		break;
	}
	return false;
}

bool InputHandler::IsMouseButtonReleased(int aMouseButton)
{
	switch (aMouseButton)
	{
	case 0:
		return myOldStates[VK_LBUTTON] && !myNewStates[VK_LBUTTON];
	case 1:
		return myOldStates[VK_RBUTTON] && !myNewStates[VK_RBUTTON];
	case 2:
		return myOldStates[VK_MBUTTON] && !myNewStates[VK_MBUTTON];
	default:
		break;
	}
	return false;
}

CU::Vector2<float> InputHandler::GetMousePos() const
{
	return myMousePos;
}

float InputHandler::GetMouseMovement()
{
	float outVal = (myMousePos.Length() - myOldMousePos.Length());
	return outVal;
}

float InputHandler::GetScrollWheelMovement()
{
	return myScrollWheelDelta;
}


