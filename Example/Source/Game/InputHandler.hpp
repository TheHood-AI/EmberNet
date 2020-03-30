#pragma once
#include <iostream>
#include <array>
#include "../TShared/Vector2.hpp"
class InputHandler
{
public:
	InputHandler();
	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void FinalizeUpdate();
	bool IsKeyDown(char aKeyCode);
	bool IsKeyHit(char aKeyCode);
	bool IsKeyReleased(char aKeyCode);

	void SetMousePos(int someX, int someY);
	bool IsMouseButtonDown(int aMouseButton);
	bool IsMouseButtonHit(int aMouseButton);
	bool IsMouseButtonReleased(int aMouseButton);
	CU::Vector2<float> GetMousePos() const;
	float GetMouseMovement();
	float GetScrollWheelMovement();
private:
	std::array<bool, 256> myNewStates;
	std::array<bool, 256> myOldStates;
	CU::Vector2<float> myMousePos;
	CU::Vector2<float> myOldMousePos;
	float myScrollWheelDelta;
	bool myWasPressed;
	bool myWasReleased;
};
